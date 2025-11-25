#include <stdio.h>
#include <player.h>
#include <raylib.h>
#include <salvar.h>

void GetSaveFileName(int slot, char *buffer) {
    sprintf(buffer, "save%d.dat", slot);
}

void SaveGame(SaveData *save, int slot) {
    char filename[15];
    GetSaveFileName(slot, filename);

    FILE *file = fopen(filename, "wb");
    if (!file) {
        printf("Erro ao salvar no slot %d.\n", slot);
        return;
    }

    fwrite(save, sizeof(SaveData), 1, file);
    fclose(file);

    printf("Save %d salvo!\n", slot);
}   

int LoadGame(SaveData *save, int slot) {
    char filename[64];
    GetSaveFileName(slot, filename);

    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Save %d não existe.\n", slot);
        return 0;
    }

    fread(save, sizeof(SaveData), 1, file);
    fclose(file);

    printf("Save %d carregado!\n", slot);
    return 1;
}

bool SaveExists(int slot) {
    char filename[64];
    GetSaveFileName(slot, filename);

    FILE *file = fopen(filename, "rb");
    if (!file) return false;
    fclose(file);
    return true;
}

int SelectSaveSlotMenu(const char *titulo) {
    int selected = -1;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        DrawText(titulo, 260, 80, 30, WHITE);

        for (int i = 0; i < 3; i++) {
            Rectangle r = { 300, 180 + i * 70, 200, 50 };

            DrawRectangleRec(r, GRAY);

            if (SaveExists(i)) DrawText(TextFormat("Slot %d (OK)", i), r.x + 20, r.y + 15, 20, WHITE);

            else DrawText(TextFormat("Slot %d (vazio)", i), r.x + 20, r.y + 15, 20, WHITE);

            if (CheckCollisionPointRec(GetMousePosition(), r) &&
                IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                selected = i;
                EndDrawing();
                return selected;
            }
        }

        DrawText("Clique num slot para selecionar", 260, 420, 20, WHITE);

        EndDrawing();
    }

    return 1; 
}

bool SaveNotEmpty(int slot) {
    char filename[64];
    sprintf(filename, "save%d.dat", slot);

    FILE *f = fopen(filename, "rb");
    if (!f) return false;  

    fseek(f, 0, SEEK_END);
    long size = ftell(f);   
    fclose(f);

    if (size == 0) return false;   

    return true;   
}


void StartNewGame(SaveData *save) {
    save->levelAtual = 0;
    save->gold = 100;
    save->tempoPassado = 0;

    save->barcomida = 1;
    save->barpoder = 1;
    save->barsaude = 1;

    save->cannonUnlocked = false;
    save->wizardUnlocked = false;

    printf("Novo jogo iniciado!\n");
}

void SaveGold(int playerGold)
{
    FILE *f = fopen("save_gold.bin", "wb");
    if (f == NULL) return;

    fwrite(&playerGold, sizeof(int), 1, f);
    fclose(f);
}

int LoadGold()
{
    FILE *f = fopen("save_gold.bin", "rb");
    if (f == NULL)
        return 0;

    int playerGold = 0;
    fread(&playerGold, sizeof(int), 1, f);
    fclose(f);

    return playerGold;
}