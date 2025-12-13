#include "dialog.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Padrão do arquivo
//ID | Pergunta | NumOpcoes | TextoOp1 | EffectTypeOp1 | ValorOp1 | DurationOp1 | TextoOp2 | EffectTypeOp2 | ValorOp2 | DurationOp2

ActiveEffect activeEffects[MAX_EFFECT_ACTIVE];
int activeCount = 0;

EffectType StringToEffect(const char *texto)
{
    if (texto == NULL) return EFFECT_NONE; 
    
    if(strstr(texto, "EFFECT_TOWER_PRICE_BONUS"))               return EFFECT_TOWER_PRICE_BONUS;
    if(strstr(texto, "EFFECT_TOWER_PRICE_PENALTY"))             return EFFECT_TOWER_PRICE_PENALTY;

    if(strstr(texto, "EFFECT_ARCHER_BONUS_DAMAGE"))             return EFFECT_ARCHER_BONUS_DAMAGE;
    if(strstr(texto, "EFFECT_WIZARD_BONUS_DAMAGE"))             return EFFECT_WIZARD_BONUS_DAMAGE;
    if(strstr(texto, "EFFECT_CANNON_BONUS_DAMAGE"))             return EFFECT_CANNON_BONUS_DAMAGE;

    if(strstr(texto, "EFFECT_ARCHER_PENALTY_DAMAGE"))           return EFFECT_ARCHER_PENALTY_DAMAGE;
    if(strstr(texto, "EFFECT_WIZARD_PENALTY_DAMAGE"))           return EFFECT_WIZARD_PENALTY_DAMAGE;
    if(strstr(texto, "EFFECT_CANNON_PENALTY_DAMAGE"))           return EFFECT_CANNON_PENALTY_DAMAGE;

    if(strstr(texto, "EFFECT_PODER_BONUS"))                     return EFFECT_PODER_BONUS;
    if(strstr(texto, "EFFECT_COMIDA_BONUS"))                    return EFFECT_COMIDA_BONUS;
    if(strstr(texto, "EFFECT_SAUDE_BONUS"))                     return EFFECT_SAUDE_BONUS;

    if(strstr(texto, "EFFECT_PODER_PENALTY"))                   return EFFECT_PODER_PENALTY;
    if(strstr(texto, "EFFECT_COMIDA_PENALTY"))                  return EFFECT_COMIDA_PENALTY;
    if(strstr(texto, "EFFECT_SAUDE_PENALTY"))                   return EFFECT_SAUDE_PENALTY;
 
    if(strstr(texto, "EFFECT_GOLD_BONUS_ENEMY"))                return EFFECT_GOLD_BONUS_ENEMY;
    if(strstr(texto, "EFFECT_GOLD_BONUS_ORC"))                  return EFFECT_GOLD_BONUS_ORC;

    if(strstr(texto, "EFFECT_GOLD_PENALTY_ENEMY"))              return EFFECT_GOLD_PENALTY_ENEMY;
    if(strstr(texto, "EFFECT_GOLD_PENALTY_ORC"))                return EFFECT_GOLD_PENALTY_ORC;

    if(strstr(texto, "EFFECT_GOLD_BONUS"))                      return EFFECT_GOLD_BONUS;
    if(strstr(texto, "EFFECT_GOLD_PENALTY"))                    return EFFECT_GOLD_PENALTY;
 
    return EFFECT_NONE;
}

Dialog *carregarPergunta(const char *arquivo, int numAleatorio) 
{
    FILE *f = fopen(arquivo, "r");
    if (!f) return NULL;

    char buffer[1024];

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

            token = strtok(NULL, "|");
            p->effect1 = StringToEffect(token);

            p->effectValue1 = atof(strtok(NULL, "|"));
            p->effectDuration1 = atoi(strtok(NULL, "|"));

            //opção 2
            if(p->numOpcoes == 2){
                token = strtok(NULL, "|");
                strcpy(p->option2, token ? token : "");

                token = strtok(NULL, "|");
                p->effect2 = StringToEffect(token);

                p->effectValue2 = atof(strtok(NULL, "|"));
                p->effectDuration2 = atoi(strtok(NULL, "|"));
            }
            printf("%d\n%d\n%d\n%d\n%d\n%f\n%f\n%d\n%d\n%s\n%s\n%s", p->active, p->effect1, p->effect2, p->effectDuration1, p->effectDuration2, p->effectValue1, p->effectValue2, p->id, p->numOpcoes, p->option1, p->option2, p->text);

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
        printf("enum1: %d\n", d->effect1);
        type = d->effect1;
        printf("type1: %d\n",type);
        value = d->effectValue1;
        duration = d->effectDuration1;
    } else {
        printf("enum2: %d\n", d->effect2);
        type = d->effect2;
        printf("type2: %d\n", type);
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

void DrawTextBoxed(Font font, const char *text, Rectangle rec, float fontSize, float spacing, Color color)
{
    int length = TextLength(text);
    float textOffsetX = 0.0f;
    float textOffsetY = 0.0f;

    float scaleFactor = fontSize / (float)font.baseSize;

    // Estado para processamento de palavras
    int k = 0;
    
    for (int i = 0; i < length; i++)
    {
        // Obter o tamanho do próximo caractere (codepoint)
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        // Se for espaço ou nova linha, verificamos se a próxima palavra cabe
        if (codepoint == ' ' || codepoint == '\n')
        {
            // Fim da palavra anterior ou quebra manual
            if (codepoint == '\n')
            {
                textOffsetX = 0;
                textOffsetY += (font.baseSize + font.baseSize/2) * scaleFactor;
            }
            else // É um espaço
            {
                // Medir a PRÓXIMA palavra para ver se cabe na linha
                float nextWordWidth = 0;
                for (int j = i + 1; j < length; j++)
                {
                    int nextCpByte = 0;
                    int nextCp = GetCodepoint(&text[j], &nextCpByte);
                    if (nextCp == ' ' || nextCp == '\n') break;
                    
                    int nextIdx = GetGlyphIndex(font, nextCp);
                    if (font.glyphs[nextIdx].advanceX == 0) nextWordWidth += (font.recs[nextIdx].width * scaleFactor + spacing);
                    else nextWordWidth += (font.glyphs[nextIdx].advanceX * scaleFactor + spacing);
                    
                    j += (nextCpByte - 1);
                }

                // Se a próxima palavra estourar a largura, pula linha AGORA
                if ((textOffsetX + nextWordWidth) > rec.width)
                {
                    textOffsetX = 0;
                    textOffsetY += (font.baseSize + font.baseSize/2) * scaleFactor;
                    continue; // Pula o desenho do espaço
                }
            }
        }

        // Desenhar o caractere atual se não estourou a altura
        if ((textOffsetY + fontSize) < rec.height)
        {
            if (codepoint != '\n')
            {
                DrawTextCodepoint(font, codepoint, (Vector2){ rec.x + textOffsetX, rec.y + textOffsetY }, fontSize, color);
                
                if (font.glyphs[index].advanceX == 0) textOffsetX += (font.recs[index].width * scaleFactor + spacing);
                else textOffsetX += (font.glyphs[index].advanceX * scaleFactor + spacing);
            }
        }
        
        i += (codepointByteCount - 1); 
    }
}

Rectangle DrawDialogBox(const char *pergunta, int screenWidth, int screenHeight) 
{
    Font font = GetFontDefault();

    int padding = 20;
    int maxBoxWidth = screenWidth - 100;
    int minBoxHeight = 140;

    Rectangle box = {
        50,
        (float)(screenHeight - minBoxHeight - 50),
        (float)maxBoxWidth,
        (float)minBoxHeight
    };

    Rectangle textArea = {
        box.x + padding,
        box.y + padding,
        box.width - padding * 2,
        box.height - padding * 3 - 40
    };

    float fontSize = 22;
    float spacing = 1.5f;

    // Mede o texto e ajusta altura se necessário
    Vector2 textSize = MeasureTextEx(font, pergunta, fontSize, spacing);
    float textLines = (textSize.x / textArea.width) + 1; 
    float estimatedTextHeight = (textSize.y * textLines) + (textLines * 10);

    if (estimatedTextHeight > textArea.height)
    {
        float extraHeight = estimatedTextHeight - textArea.height;
        box.height += extraHeight + 20; // +20 de folga
        box.y = screenHeight - box.height - 50; // Reajusta o Y para crescer para cima

        // Atualiza area de texto baseada na nova caixa
        textArea.y = box.y + padding;
        textArea.height = estimatedTextHeight;
    }

    // Sombra
    DrawRectangleRounded(
        (Rectangle){ box.x + 4, box.y + 4, box.width, box.height },
        0.08f, 8, Fade(BLACK, 0.4f)
    );

    // Caixa principal
    DrawRectangleRounded(box, 0.08f, 8, (Color){30, 30, 30, 245});
    DrawRectangleRoundedLinesEx(box, 0.08f, 8, 2, GOLD);

    // Texto
    DrawTextBoxed(font, pergunta, textArea, fontSize, spacing, RAYWHITE);

    return box; // RETORNA a caixa final calculada

}

void DrawCenteredButton(Rectangle btn, const char* text, Color baseColor, Color outlineColor) {
        DrawRectangleRounded(btn, 0.2f, 6, baseColor);
        DrawRectangleRoundedLinesEx(btn, 0.2f, 6, 2.0f, outlineColor);
        
        // Centralização do texto
        Font font = GetFontDefault();
        float fontSize = 18.0f;
        float spacing = 1.0f;

        Vector2 txtSize = MeasureTextEx(font, text, fontSize, spacing);
        Vector2 textPos = {
            btn.x + (btn.width - txtSize.x) / 2.0f,
            btn.y + (btn.height - txtSize.y) / 2.0f
        };

        DrawTextBoxed(font, text, btn, fontSize, spacing, RAYWHITE);
    }

int DrawDialogButtons(Dialog *dialogo, Rectangle parentBox)
{
    if (!dialogo) return 0;

    int padding = 20;
    int buttonWidth = 350;
    int buttonHeight = 80;

    int y = parentBox.y + parentBox.height - buttonHeight - padding;
    Vector2 mouse = GetMousePosition();

    if (dialogo->numOpcoes == 2)
    {
        Rectangle b1 = { parentBox.x + padding, (float)y, (float)buttonWidth, (float)buttonHeight };
        Rectangle b2 = { parentBox.x + parentBox.width - buttonWidth - padding, (float)y, (float)buttonWidth, (float)buttonHeight };
        Color colorB1 = DARKBLUE;
        Color lineB1 = SKYBLUE;
        
        Color colorB2 = DARKGREEN;
        Color lineB2 = LIME;

        if (CheckCollisionPointRec(mouse, b1)){
            colorB1 = (Color){0, 0, 50, 255}; // azul ainda mais escuro
            lineB1 = WHITE; 

            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return 1;
        }

        if (CheckCollisionPointRec(mouse, b2)){
            colorB2 = (Color){0, 50, 0, 255}; // Verde mais escuro
            lineB2 = WHITE;
            
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return 2;   
        }

        DrawCenteredButton(b1, dialogo->option1, colorB1, lineB1);
        DrawCenteredButton(b2, dialogo->option2, colorB2, lineB2);
    }
    else if (dialogo->numOpcoes == 1)
    {
        Rectangle b = {
            parentBox.x + (parentBox.width - buttonWidth) / 2,
            (float)y,
            (float)buttonWidth,
            (float)buttonHeight
        };
        Color colorB = MAROON;
        Color lineB = RED;

        if (CheckCollisionPointRec(mouse, b)){
            colorB = (Color){50, 0, 0, 255}; // Vermelho escuro/Preto
            lineB = WHITE;
            
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return 1;
            
        }

        DrawCenteredButton(b, dialogo->option1, colorB, lineB);

    }

    return 0;

}

int DrawDialogScreen(Dialog *dialogo, int screenWidth, int screenHeight)
{
    if (dialogo == NULL) return 0;

    Rectangle finalBox = DrawDialogBox(dialogo->text, screenWidth, screenHeight);
    
    return DrawDialogButtons(dialogo, finalBox);
}