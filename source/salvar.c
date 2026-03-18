#include "gamedata.h"
#include <stdio.h>
#include <player.h>
#include <raylib.h>
#include <salvar.h>
#include <math.h>
#include <sys/types.h>

SaveData save;

int saveSelection(SaveData *save,Save *menu);
void DrawSaveMenuSprites(Texture2D archerTexture) ;
int SelectSaveSlotMenu(Vector2 *mousePos, bool *isNovoJogo);

void GetSaveFileName(int slot, char *buffer) {
    sprintf(buffer, "save%d.bin", slot);
}

void SaveGame(SaveData *save, int slot) {
    char filename[64];
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



State updateSaveMenu(void *data){
	Save* menu = (Save*)data;

	menu->mousePos = GetMousePosition();
	if(saveSelection(&save,menu) == -1) return KEEP;

	return TUTORIAL;

}

int saveSelection(SaveData *save,Save *menu){
	Player *player = menu->player;
	bool isNovoJogo;
	int slot = -1;

	if((slot = SelectSaveSlotMenu(&menu->mousePos,&isNovoJogo)) == -1) return -1;

	if (SaveNotEmpty(slot) && !isNovoJogo) {
		LoadGame(save, slot);
		puts("Save Carregado!");
	}else{
		StartNewGame(save);
	}

	player->gold = save->gold;
	player->food = save->barcomida;
	player->health = save->barsaude;
	player->power = save->barpoder;
	player->unlockedCannon = save->cannonUnlocked;
	player->unlockedWizard = save->wizardUnlocked;
	player->level = save->levelAtual;
	player->timePassed = save->tempoPassado;

	return slot;
}

int SelectSaveSlotMenu(Vector2 *mousePos, bool *isNovoJogo){
	for (int i = 0; i < 3; i++) {

		Rectangle r = { 
			(GetScreenWidth() - 380) / 2.0f,
			180 + i * 90,
			380,
			70
		};

		bool mouseOver = CheckCollisionPointRec(*mousePos, r);

		if (mouseOver && (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))) {
			if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
				*isNovoJogo = false;
			}else{
				*isNovoJogo = true;
			}
	
			return i;
		}

	}
	return -1;
}

void drawSaveMenu(void *data){
	Save* saveMenu = (Save*)data;

        ClearBackground((Color){15, 15, 20, 255});

        DrawSaveMenuSprites(*saveMenu->sprites);

        DrawText(saveMenu->title,
                 (GetScreenWidth() - MeasureText(saveMenu->title, 40)) / 2,
                 60,
                 40,
                 (Color){230, 230, 255, 255});

        DrawText("Clique com o botão direito para substituir um save",
                 (GetScreenWidth() - MeasureText("Clique com o botão direito para substituir um save", 20)) / 2,
                 110,
                 20,
                 (Color){180, 180, 200, 255});

	for (int i = 0; i < 3; i++) {

		Rectangle r = { 
			(GetScreenWidth() - 380) / 2.0f,
			180 + i * 90,
			380,
			70
		};

		bool mouseOver = CheckCollisionPointRec(saveMenu->mousePos, r);

		Color bg = mouseOver ? (Color){60, 60, 90, 255} : (Color){40, 40, 60, 255};

		DrawRectangleRounded(r, 0.2f, 8, bg);
		DrawRectangleLinesEx(r,
			2,
			mouseOver ? (Color){150, 150, 255, 255} : (Color){90, 90, 130, 255});

		if (SaveExists(i)){
			DrawText(TextFormat("Slot %d (OK)", i), r.x + 20, r.y + 15, 20, WHITE);
		}else{
			DrawText(TextFormat("Slot %d (vazio)", i), r.x + 20, r.y + 15, 20, WHITE);
		}
	}

	float brilho = (sinf(GetTime() * 2) + 1) / 2;
	unsigned char alpha = brilho * 255;

	const char *msg = "Selecione um slot  |  F11: alternar fullscreen";
	DrawText(msg,
		(GetScreenWidth() - MeasureText(msg, 20)) / 2,
		GetScreenHeight() - 70,
		20,
		(Color){200, 200, 220, alpha});

}

void UpdateSave(SaveData *save, float barComida, float barPoder, float barSaude, int level, int tempoPassado) {
    save->gold = playerGold;
    save->levelAtual = level;
    save->barcomida = barComida;
    save->barpoder = barPoder;
    save->barsaude = barSaude;
    save->cannonUnlocked = cannonUnlocked;
    save->wizardUnlocked = wizardUnlocked;
    save->tempoPassado = tempoPassado; 
}

bool SaveNotEmpty(int slot) {
    char filename[64];
    sprintf(filename, "save%d.bin", slot);

    FILE *f = fopen(filename, "rb");
    if (!f) return false;  

    fseek(f, 0, SEEK_END);
    long size = ftell(f);   
    fclose(f);

    if (size == 0) return false;   

    return true;   
}



void DeleteSave(int slot) {
    char filename[64];
    sprintf(filename, "save%d.bin", slot);

    if (remove(filename) == 0) {
        printf("Save %d deletado!\n", slot);
    } else {
        printf("Erro ao deletar o save %d.\n", slot);
    }
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

void DrawSaveMenuSprites(Texture2D archerTexture) {
    int totalFramesShot = 14;
    int frameShot = 10;
    int larguraFrameShot = archerTexture.width / totalFramesShot;

    Rectangle srcShot = {
        larguraFrameShot * frameShot,
        0,
        larguraFrameShot,
        archerTexture.height
    };

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    float size = screenH * 0.6f;  

    Rectangle dstShot = {
        screenW * 0.25f - size / 2.0f,
        screenH * 0.50f - size / 2.0f,
        size,
        size
    };

    dstShot.x += -100;
    DrawTexturePro(archerTexture, srcShot, dstShot, (Vector2){0,0}, 0, WHITE);

    float offsetRight = 100;

    dstShot = (Rectangle){
        screenW * 0.75f - size / 2.0f + offsetRight,
        screenH * 0.50f - size / 2.0f,
        size,
        size
    };

    srcShot.width = -larguraFrameShot;

    DrawTexturePro(archerTexture, srcShot, dstShot, (Vector2){0,0}, 0, WHITE);
}
