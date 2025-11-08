#include <raylib.h>
#include "HUD.h"
#include "player.h"

static bool hudVisible = false;
static Vector2 hudPos;
static int selectedTower = -1;
static UnitType chosenUnit = UNIT_NONE;
static int hudWidth = 150;
static int hudHeight = 150; // aumentado para caber mais botões

// Texturas dos ícones
static Texture2D archerIcon;
static Texture2D wizardIcon;
static bool iconsLoaded = false;

void HUD_LoadIcons(void) {
    if (!iconsLoaded) {
        Image a = LoadImage("assets/ArcherIcon.png");
        Image w = LoadImage("assets/Shotwizard.png");
        archerIcon = LoadTextureFromImage(a);
        wizardIcon = LoadTextureFromImage(w);
        UnloadImage(a);
        UnloadImage(w);
        iconsLoaded = true;
    }
}

void HUD_ShowAt(Vector2 pos, int towerIndex) {
    HUD_LoadIcons();
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
        Rectangle wizardBtn = { hudPos.x, hudPos.y + hudHeight / 2, hudWidth, hudHeight / 2 };

        if (CheckCollisionPointRec(mouse, archerBtn)) {
            chosenUnit = UNIT_ARCHER;
            hudVisible = false;
        }
        else if (CheckCollisionPointRec(mouse, wizardBtn)) {
            chosenUnit = UNIT_WIZARD;
            hudVisible = false;
        }
    }
}

void HUD_Draw(void) {
    if (!hudVisible) return;

    DrawRectangle((int)hudPos.x, (int)hudPos.y, hudWidth, hudHeight, Fade(LIGHTGRAY, 0.9f));
    DrawRectangleLines((int)hudPos.x, (int)hudPos.y, hudWidth, hudHeight, DARKGRAY);

    DrawRectangleLines((int)hudPos.x, (int)hudPos.y, hudWidth, hudHeight / 2, DARKGRAY);
    DrawRectangleLines((int)hudPos.x, (int)hudPos.y + hudHeight / 2, hudWidth, hudHeight / 2, DARKGRAY);

    DrawText("Archer", hudPos.x + 10, hudPos.y + 10, 20, BLACK);
    DrawText("Wizard", hudPos.x + 10, hudPos.y + 10 + hudHeight / 2, 20, BLACK);

    if (iconsLoaded) {
        DrawTexturePro(archerIcon, 
            (Rectangle){0, 0, archerIcon.width, archerIcon.height}, 
            (Rectangle){hudPos.x + hudWidth - 45, hudPos.y + 10, 32, 32}, 
            (Vector2){0, 0}, 0, WHITE);
        DrawTexturePro(wizardIcon, 
            (Rectangle){0, 0, wizardIcon.width / 8, wizardIcon.height}, // primeiro frame
            (Rectangle){hudPos.x + hudWidth - 45, hudPos.y + 10 + hudHeight / 2, 32, 32}, 
            (Vector2){0, 0}, 0, WHITE);
    }
}

void desenharRetangulo(int alturaImagem, int posYSprite){
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int posY = posYSprite - 5; 
    int alturaRetangulo = screenHeight - posY;          
    DrawRectangle(0, posY, screenWidth, alturaRetangulo, GRAY);
}

void desenharRei(Texture2D reiTextura, int posx, int posy){
    DrawTexture(reiTextura, posx, posy, WHITE);
}