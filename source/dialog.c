#include "dialog.h"
#include "raylib.h"

int carregarPerguntas(const char *arquivo, Dialog *lista, int maxPerguntas) {
    FILE *f = fopen(arquivo, "r");
    if (!f) return 0;

    int count = 0;
    while (!feof(f) && count < maxPerguntas) {
        Pergunta p;
        char buffer[512];

        if (fgets(buffer, sizeof(buffer), f)) {
            // Remover \n
            buffer[strcspn(buffer, "\n")] = 0;

            // Quebra a linha usando strtok
            char *token = strtok(buffer, "|");
            p.id = atoi(token);

            token = strtok(NULL, "|");
            strcpy(p.pergunta, token);

            token = strtok(NULL, "|");
            p.numOpcoes = atoi(token);

            token = strtok(NULL, "|");
            strcpy(p.opcao1, token);

            token = strtok(NULL, "|");
            if (token)
                strcpy(p.opcao2, token);
            else
                p.opcao2[0] = '\0';

            lista[count++] = p;
        }
    }

    fclose(f);
    return count;
}

Dialog escolherPerguntaAleatoria(Dialog *lista, int n) {
    int idx = GetRandomValue(0, n - 1);
    return lista[idx];
}


void botoes(DialogType tipoDeDialogo, bool isOpOneGood, bool isOpTwoGood, int screenWidth, int screenHeight) {
    int boxWidth = screenWidth - 100;
    int boxHeight = 150;
    int boxX = 50;
    int boxY = screenHeight - boxHeight - 50;

    int buttonWidth = 150;
    int buttonHeight = 40;
    int padding = 20;

    if (tipoDeDialogo == DIALOG_CHOICES) {
        // Botão opção 1
        Rectangle button1 = { boxX + padding, boxY + boxHeight - buttonHeight - padding, buttonWidth, buttonHeight };
        DrawRectangleRec(button1, isOpOneGood ? GREEN : RED);
        DrawText("Opção 1", button1.x + 20, button1.y + 10, 20, WHITE);

        // Botão opção 2
        Rectangle button2 = { boxX + boxWidth - buttonWidth - padding, boxY + boxHeight - buttonHeight - padding, buttonWidth, buttonHeight };
        DrawRectangleRec(button2, isOpTwoGood ? GREEN : RED);
        DrawText("Opção 2", button2.x + 20, button2.y + 10, 20, WHITE);

    } else if (tipoDeDialogo == DIALOG_SIMPLE) {
        // Botão única opção
        Rectangle button = { boxX + (boxWidth - buttonWidth) / 2, boxY + boxHeight - buttonHeight - padding, buttonWidth, buttonHeight };
        DrawRectangleRec(button, BLUE);
        DrawText("Continuar", button.x + 20, button.y + 10, 20, WHITE);
    }
}

void DrawDialogBox(const char *pergunta, int screenWidth, int screenHeight) {
    int boxWidth = screenWidth - 100;
    int boxHeight = 150;
    int boxX = 50;
    int boxY = screenHeight - boxHeight - 50;

    // Desenha a caixa de diálogo
    DrawRectangle(boxX, boxY, boxWidth, boxHeight, DARKGRAY);
    DrawRectangleLines(boxX, boxY, boxWidth, boxHeight, BLACK);

    // Configura o texto
    int fontSize = 20;
    int padding = 20;
    Rectangle textBox = { boxX + padding, boxY + padding, boxWidth - 2 * padding, boxHeight - 2 * padding };

    // Desenha o texto dentro da caixa de diálogo
    DrawTextEx(GetFontDefault(), pergunta, (Vector2){textBox.x, textBox.y}, fontSize, 1.0f, WHITE);

}