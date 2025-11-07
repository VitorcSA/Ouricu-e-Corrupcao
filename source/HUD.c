#include <raylib.h>
#include "HUD.h"
#include "player.h"

static bool hudVisible = false;
static Vector2 hudPos;
static int selectedTower = -1;
static UnitType chosenUnit = UNIT_NONE;
static int hudWidth = 150;
static int hudHeight = 100;

void HUD_ShowAt(Vector2 pos, int towerIndex) {
    hudVisible = true;
    hudPos = (Vector2){pos.x, pos.y - hudHeight - 10};
    selectedTower = towerIndex;
    chosenUnit = UNIT_NONE;
}

bool HUD_IsActive(void) {
    return hudVisible;
}

int HUD_GetSelectedTower(void) {
    return selectedTower;
}

UnitType HUD_GetSelectedUnit(void) {
    return chosenUnit;
}

void HUD_Update(void) {
    if (!hudVisible) return;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();

        Rectangle archerBtn = { hudPos.x, hudPos.y, hudWidth, hudHeight / 2 };
        if (CheckCollisionPointRec(mouse, archerBtn)) {
            chosenUnit = UNIT_ARCHER;
            hudVisible = false; // Fecha HUD
        }

        // outros botões virão aqui depois
    }
}

void HUD_Draw(void) {
    if (!hudVisible) return;

    DrawRectangle((int)hudPos.x, (int)hudPos.y, hudWidth, hudHeight, Fade(LIGHTGRAY, 0.9f));
    DrawRectangleLines((int)hudPos.x, (int)hudPos.y, hudWidth, hudHeight / 2, DARKGRAY);
    DrawText("Archer", hudPos.x + 10, hudPos.y + 10, 20, BLACK);
}

void desenharRetangulo(int alturaImagem, int posYSprite){
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    int posY = posYSprite - 5; 
    int alturaRetangulo = screenHeight - posY;          

    DrawRectangle(0, posY, screenWidth, alturaRetangulo, GRAY);
}

void desenharRei(Texture2D reiTextura, int posx, int posy){
    DrawTexture(reiTextura,
            posx,
            posy,
            WHITE);
}