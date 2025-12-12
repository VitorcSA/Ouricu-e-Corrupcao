#include "dialog.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

            p->id = id;

            token = strtok(NULL, "|");
            strcpy(p->text, token ? token : "");

            token = strtok(NULL, "|");
            p->numOpcoes = token ? atoi(token) : 0;

            token = strtok(NULL, "|");
            strcpy(p->option1, token ? token : "");

            token = strtok(NULL, "|");
            strcpy(p->option2, token ? token : "");

            fclose(f);
            return p;   // Achou e retorna imediatamente
        }
    }

    fclose(f);
    return NULL; // Caso não encontre
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
