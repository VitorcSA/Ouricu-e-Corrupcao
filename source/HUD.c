#include <raylib.h>
#include "HUD.h"
#include "player.h"

static bool hudVisible = false;
static Vector2 hudPos;
static int selectedTower = -1;
static UnitType chosenUnit = UNIT_NONE;
static int hudWidth = 150;
static int hudHeight = 220;

void HUD_ShowAt(Vector2 pos, int towerIndex) {
    hudVisible = true;
    hudPos = (Vector2){pos.x, pos.y - hudHeight - 10};
    selectedTower = towerIndex;
    chosenUnit = UNIT_NONE;
}

bool HUD_IsActive(void) { return hudVisible; }
int HUD_GetSelectedTower(void) { return selectedTower; }
UnitType HUD_GetSelectedUnit(void) { return chosenUnit; }

void HUD_Update(void) {
    if (!hudVisible) return;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();

        int section = hudHeight / 3;
        Rectangle archerBtn = { hudPos.x, hudPos.y, hudWidth, section };
        Rectangle wizardBtn = { hudPos.x, hudPos.y + section, hudWidth, section };
        Rectangle cannonBtn = { hudPos.x, hudPos.y + 2 * section, hudWidth, section };

        if (CheckCollisionPointRec(mouse, archerBtn)) {
            chosenUnit = UNIT_ARCHER;
            hudVisible = false;
        } else if (CheckCollisionPointRec(mouse, wizardBtn)) {
            chosenUnit = UNIT_WIZARD;
            hudVisible = false;
        } else if (CheckCollisionPointRec(mouse, cannonBtn)) {
            chosenUnit = UNIT_CANNON;
            hudVisible = false;
        }
    }
}

void HUD_Draw(void) {
    if (!hudVisible) return;

    DrawRectangleRec((Rectangle){hudPos.x, hudPos.y, hudWidth, hudHeight}, Fade(LIGHTGRAY, 0.95f));
    DrawRectangleLines(hudPos.x, hudPos.y, hudWidth, hudHeight, DARKGRAY);

    int section = hudHeight / 3;

    DrawRectangleLines(hudPos.x, hudPos.y, hudWidth, section, DARKGRAY);
    DrawRectangleLines(hudPos.x, hudPos.y + section, hudWidth, section, DARKGRAY);
    DrawRectangleLines(hudPos.x, hudPos.y + 2 * section, hudWidth, section, DARKGRAY);

    DrawText("Archer", hudPos.x + 10, hudPos.y + 10, 20, BLACK);
    DrawText("Wizard", hudPos.x + 10, hudPos.y + section + 10, 20, BLACK);
    DrawText("Cannon", hudPos.x + 10, hudPos.y + 2 * section + 10, 20, BLACK);
}

void InitGoldHUD(GoldHUD *hud)
{
    hud->gold = 0;
    hud->rect.width = 140;  // ajustar se necessário
    hud->rect.height = 40;  // altura suficiente para o texto
    hud->rect.x = GetScreenWidth() - hud->rect.width - 10;
    hud->rect.y = GetScreenHeight() - hud->rect.height - 10;

    Image coinImg = LoadImage("assets/coin.png");
    ImageResize(&coinImg, 24, 24);
    hud->coinTexture = LoadTextureFromImage(coinImg);
    UnloadImage(coinImg);
}

void UpdateGoldHUD(GoldHUD *hud, int playerGold)
{
    hud->gold = playerGold;
}

void DrawGoldHUD(GoldHUD *hud)
{
    DrawRectangleRec(hud->rect, (Color){200, 200, 200, 150});
    DrawText(TextFormat("Gold: %d", playerGold), hud->rect.x + 10, hud->rect.y + 10, 20, GOLD);
    DrawTexture(hud->coinTexture, hud->rect.x + 110, hud->rect.y + 8, WHITE);
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