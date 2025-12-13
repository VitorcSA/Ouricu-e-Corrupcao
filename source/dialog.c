#include "dialog.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Padrão do arquivo
//ID | Pergunta | NumOpcoes | TextoOp1 | EffectTypeOp1 | ValorOp1 | DurationOp1 | TextoOp2 | EffectTypeOp2 | ValorOp2 | DurationOp2

ActiveEffect activeEffects[MAX_EFFECT_ACTIVE];
int activeCount = 0;

Dialog *carregarPergunta(const char *arquivo, int numAleatorio) 
{
    FILE *f = fopen(arquivo, "r");
    if (!f) {
        return NULL;
    }

    char buffer[512];

    while (fgets(buffer, sizeof(buffer), f)) {

        // Remove \n
        buffer[strcspn(buffer, "\n")] = 0;

        // Quebra por token
        char *token = strtok(buffer, "|");
        if (!token) continue;

        int id = atoi(token);

        // Checa se é o ID desejado
        if (id == numAleatorio) {

            Dialog *p = malloc(sizeof(Dialog));
            if (!p) {
                fclose(f);
                return NULL;
            }
            memset(p, 0, sizeof(Dialog));

            p->id = id;

            token = strtok(NULL, "|");
            strcpy(p->text, token ? token : "");

            token = strtok(NULL, "|");
            p->numOpcoes = token ? atoi(token) : 0;

            //opção 1
            token = strtok(NULL, "|");
            strcpy(p->option1, token ? token : "");
            p->effect1 = (EffectType)atoi(strtok(NULL, "|"));
            p->effectValue1 = atof(strtok(NULL, "|"));
            p->effectDuration1 = atoi(strtok(NULL, "|"));

            //opção 2
            if(p->numOpcoes == 2){
                token = strtok(NULL, "|");
                strcpy(p->option2, token ? token : "");
                p->effect2 = (EffectType)atoi(strtok(NULL, "|"));
                p->effectValue2 = atof(strtok(NULL, "|"));
                p->effectDuration2 = atoi(strtok(NULL, "|"));
            }

            fclose(f);
            return p;   // Achou e retorna imediatamente
        }
    }

    fclose(f);
    return NULL; // Caso não encontre
}

// Adicione ponteiros para os recursos que podem ser alterados imediatamente
void ApplyDialogEffect(Dialog *d, int option, int *gold, float *vida, float *comida, float *poder)
{
    EffectType type;
    float value;
    int duration;

    if (option == 1) {
        type = d->effect1;
        value = d->effectValue1;
        duration = d->effectDuration1;
    } else {
        type = d->effect2;
        value = d->effectValue2;
        duration = d->effectDuration2;
    }

    // LÓGICA 1: Efeitos Imediatos (Duração irrelevante ou 0)
    // Eles alteram a variável diretamente e não entram na lista de activeEffects
    switch (type) {
        case EFFECT_GOLD_BONUS:
            *gold += (int)value;
            printf("Ganhou %d de ouro!\n", (int)value);
            return; // Retorna para não adicionar à lista
        case EFFECT_GOLD_PENALTY:
            *gold -= (int)value;
            if (*gold < 0) *gold = 0;
            return;
        case EFFECT_SAUDE_BONUS:
            *vida += value; // Assumindo que você trata o cap máximo depois
            return;
        case EFFECT_SAUDE_PENALTY:
            *vida -= value;
            return;
        // Adicione casos para Comida e Poder aqui...
        default:
            break; // Se não for imediato, segue para a lógica de duração
    }

    // LÓGICA 2: Efeitos Duradouros (Buffs/Debuffs)
    if (duration > 0) {
        ActiveEffect newEffect;
        newEffect.type = type;
        newEffect.value = value;
        newEffect.duration = duration;
        newEffect.active = true;

        if (activeCount < MAX_EFFECT_ACTIVE) {
            activeEffects[activeCount++] = newEffect;
            printf("Efeito %d aplicado por %d ondas.\n", type, duration);
        }
    }
}

void UpdateEffects()
{
    for (int i = 0; i < activeCount; i++)
    {
        if (!activeEffects[i].active)
            continue;

        // Diminui duração
        if (activeEffects[i].duration > 0)
            activeEffects[i].duration--;

        // Quando chega a zero → desativa o efeito
        if (activeEffects[i].duration == 0)
        {
            activeEffects[i].active = false;
            printf("Efeito %d terminou.\n", activeEffects[i].type);
        }
    }
}

void resetEffects(){
    for(int i = 0; i < activeCount; i++){
        if(activeEffects[i].active) activeEffects[i].active = false;
    }
}

void CleanupEffects()
{
    for (int i = 0; i < activeCount; i++)
    {
        if (!activeEffects[i].active)
        {
            activeEffects[i] = activeEffects[activeCount - 1];
            activeCount--;
            i--;
        }
    }
}

float GetTotalModifier(EffectType typeWanted)
{
    float total = 0;

    for (int i = 0; i < activeCount; i++)
    {
        if (activeEffects[i].active && activeEffects[i].type == typeWanted)
        {
            total += activeEffects[i].value;
        }
    }
    return total;
}

void DrawDialogBox(const char *pergunta, int screenWidth, int screenHeight) 
{
    int boxWidth = screenWidth - 100;
    int boxHeight = 150;
    int boxX = 50;
    int boxY = screenHeight - boxHeight - 50;

    DrawRectangle(boxX, boxY, boxWidth, boxHeight, DARKGRAY);
    DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, BLACK);

    int fontSize = 20;
    int padding = 20;

    DrawTextEx(GetFontDefault(), pergunta,
               (Vector2){boxX + padding, boxY + padding},
               fontSize, 1, WHITE);
}

void DrawDialogButtons(Dialog *dialogo, int screenWidth, int screenHeight)
{
    int boxWidth = screenWidth - 100;
    int boxHeight = 150;
    int boxX = 50;
    int boxY = screenHeight - boxHeight - 50;

    int buttonWidth = 150;
    int buttonHeight = 40;
    int padding = 20;

    if (dialogo->numOpcoes == 2) {

        // Botão opção 1
        Rectangle button1 = { 
            boxX + padding, 
            boxY + boxHeight - buttonHeight - padding, 
            buttonWidth, 
            buttonHeight 
        };

        DrawRectangleRec(button1, GRAY);
        DrawText(dialogo->option1, button1.x + 15, button1.y + 10, 20, WHITE);

        // Botão opção 2
        Rectangle button2 = { 
            boxX + boxWidth - buttonWidth - padding, 
            boxY + boxHeight - buttonHeight - padding, 
            buttonWidth, 
            buttonHeight 
        };

        DrawRectangleRec(button2, GRAY);
        DrawText(dialogo->option2, button2.x + 15, button2.y + 10, 20, WHITE);

    } 
    else if (dialogo->numOpcoes == 1) 
    {
        Rectangle button = { 
            boxX + (boxWidth - buttonWidth) / 2, 
            boxY + boxHeight - buttonHeight - padding, 
            buttonWidth, 
            buttonHeight 
        };

        DrawRectangleRec(button, BLUE);
        DrawText(dialogo->option1, button.x + 15, button.y + 10, 20, WHITE);
    }
}

void DrawDialogScreen(Dialog *dialogo, int screenWidth, int screenHeight)
{
    if (dialogo == NULL) return;

    DrawDialogBox(dialogo->text, screenWidth, screenHeight);
    DrawDialogButtons(dialogo, screenWidth, screenHeight);
}

int CheckDialogClick(Dialog *dialogo, int screenWidth, int screenHeight){
    if (dialogo == NULL) return 0;

    int boxWidth = screenWidth - 100;
    int boxHeight = 150;
    int boxX = 50;
    int boxY = screenHeight - boxHeight - 50;

    int buttonWidth = 150;
    int buttonHeight = 40;
    int padding = 20;

    Vector2 mouse = GetMousePosition();

    if (dialogo->numOpcoes == 2) {

        // Botão opção 1
        Rectangle button1 = { 
            boxX + padding, 
            boxY + boxHeight - buttonHeight - padding, 
            buttonWidth, 
            buttonHeight 
        };

       if (CheckCollisionPointRec(mouse, button1) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
        {
            return 1;
        }


        // Botão opção 2
        Rectangle button2 = { 
            boxX + boxWidth - buttonWidth - padding, 
            boxY + boxHeight - buttonHeight - padding, 
            buttonWidth, 
            buttonHeight 
        };

        if (CheckCollisionPointRec(mouse, button2) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
        {
            return 2;
        }


    } 
    else if (dialogo->numOpcoes == 1) 
    {
        Rectangle button = { 
            boxX + (boxWidth - buttonWidth) / 2, 
            boxY + boxHeight - buttonHeight - padding, 
            buttonWidth, 
            buttonHeight 
        };

        if (CheckCollisionPointRec(mouse, button) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            return 1;
        }
    
    }
    return 0;
}
