#include <raylib.h>
#include "HUD.h"
#include "player.h"
#include <stdio.h>

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

void DrawGoldHUDAt(GoldHUD *hud) {
    float x = 20;
    float y = 20;
    float width = 220;
    float height = 60;

    DrawRectangleRounded((Rectangle){x, y, width, height}, 0.2f, 10, (Color){30, 30, 40, 180});
    DrawRectangleRoundedLines((Rectangle){x, y, width, height}, 0.2f, 10, Fade(GOLD, 0.6f));

    const int fontSize = 24;
    const char *label = "Gold";
    char goldText[32];
    sprintf(goldText, "%d", hud->gold);

    int goldLabelWidth = MeasureText(label, fontSize);
    int goldValueWidth = MeasureText(goldText, fontSize);
    int coinWidth = hud->coinTexture.width * 1.5f;
    int spacing = 10;

    float totalWidth = coinWidth + spacing + goldLabelWidth + spacing + goldValueWidth;
    if (totalWidth > width - 20) totalWidth = width - 20;

    float startX = x + 15;
    float centerY = y + (height - fontSize) / 2 + 2;

    float coinX = startX;
    float labelX = coinX + coinWidth + spacing;
    float valueX = labelX + goldLabelWidth + spacing;

    DrawTextureEx(
        hud->coinTexture,
        (Vector2){coinX, y + (height - hud->coinTexture.height * 1.5f) / 2},
        0.0f,
        1.5f,
        WHITE
    );

    DrawText(label, labelX, centerY, fontSize, YELLOW);
    DrawText(TextFormat("%d", playerGold), valueX, centerY, fontSize, RAYWHITE);

    // Desenhar a moeda à direita
    if (hud->coinTexture.id > 0) {
        DrawTextureEx(
            hud->coinTexture,
            (Vector2){startX, y + (height - hud->coinTexture.height * 1.5f) / 2},
            0.0f,
            1.5f,
            WHITE
        );
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