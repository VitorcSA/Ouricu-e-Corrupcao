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

float barsaude = 1.0f;
float barcomida = 1.0f;
float barinfra = 1.0f;

Texture2D cobreTexture;
Texture2D ferroTexture;
Texture2D bronzeTexture;
Texture2D prataTexture;
Texture2D ouroTexture;
Texture2D diamanteTexture;

void InitRanking(){
    Image cobre = LoadImage("assets/Ranking/cobre.png");
    cobreTexture = LoadTextureFromImage(cobre);
    UnloadImage(cobre);

    Image ferro = LoadImage("assets/Ranking/ferro.png");
    ferroTexture = LoadTextureFromImage(ferro);
    UnloadImage(ferro);

    Image bronze = LoadImage("assets/Ranking/bronze.png");
    bronzeTexture = LoadTextureFromImage(bronze);
    UnloadImage(bronze);

    Image prata = LoadImage("assets/Ranking/prata.png");
    prataTexture = LoadTextureFromImage(prata);
    UnloadImage(prata);

    Image ouro = LoadImage("assets/Ranking/ouro.png");
    ouroTexture = LoadTextureFromImage(ouro);
    UnloadImage(ouro);

    Image diamante = LoadImage("assets/Ranking/diamante.png");
    diamanteTexture = LoadTextureFromImage(diamante);
    UnloadImage(diamante);
}

void HUD_ShowAt(Vector2 pos, int towerIndex) {
    hudVisible = true;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    hudWidth  = screenW * 0.18f;
    hudHeight = screenH * 0.30f;

    hudPos = (Vector2){ pos.x, 
                        pos.y - hudHeight * 0.5f };

    if (hudPos.x + hudWidth > screenW)  hudPos.x = screenW - hudWidth - 10;
    if (hudPos.y + hudHeight > screenH) hudPos.y = screenH - hudHeight - 10;
    if (hudPos.y < 0)                   hudPos.y = 10;
    if (hudPos.x < 0)                   hudPos.x = 10;

    selectedTower = towerIndex;

    chosenUnit = UNIT_NONE;
}

bool HUD_IsActive(void) { return hudVisible; }
int HUD_GetSelectedTower(void) { return selectedTower; }
UnitType HUD_GetSelectedUnit(void) { return chosenUnit; }

void HUD_Update(void) {
    if (!hudVisible) return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    hudWidth  = screenW * 0.18f;
    hudHeight = screenH * 0.30f;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();

        int section = hudHeight / 3;
        Rectangle archerBtn = { hudPos.x, hudPos.y, hudWidth, section };
        Rectangle wizardBtn = { hudPos.x, hudPos.y + section, hudWidth, section };
        Rectangle cannonBtn = { hudPos.x, hudPos.y + 2 * section, hudWidth, section };

        if (CheckCollisionPointRec(mouse, archerBtn)) {
                chosenUnit = UNIT_ARCHER;
                hudVisible = false;
    }

        if (CheckCollisionPointRec(mouse, wizardBtn)) {
            if (wizardUnlocked) {
                chosenUnit = UNIT_WIZARD;
                hudVisible = false;
    }
}
        else if (CheckCollisionPointRec(mouse, cannonBtn)) {
            if (cannonUnlocked) {
                chosenUnit = UNIT_CANNON;
                hudVisible = false;
    }
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

if (wizardUnlocked)
    DrawText("Wizard", hudPos.x + 10, hudPos.y + section + 10, 20, BLACK);
else
    DrawText("Wizard (Bloqueado)", hudPos.x + 10, hudPos.y + section + 10, 20, RED);

if (cannonUnlocked)
    DrawText("Cannon", hudPos.x + 10, hudPos.y + 2*section + 10, 20, BLACK);
else
    DrawText("Cannon (Bloqueado)", hudPos.x + 10, hudPos.y + 2*section + 10, 20, RED);

}

void SaveGold(int playerGold)
{
    FILE *f = fopen("save_gold.dat", "wb");
    if (f == NULL) return;

    fwrite(&playerGold, sizeof(int), 1, f);
    fclose(f);
}

int LoadGold()
{
    FILE *f = fopen("save_gold.dat", "rb");
    if (f == NULL)
        return 0;

    int playerGold = 0;
    fread(&playerGold, sizeof(int), 1, f);
    fclose(f);

    return playerGold;
}

void InitGoldHUD(GoldHUD *hud)
{
    hud->gold = 0;
    hud->rect.width = 140;
    hud->rect.height = 40;
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

void DrawHorizontalBar(float x, float y, float width, float height, float value)
{
    if (value < 0) value = 0;
    if (value > 1) value = 1;

    Color col = (value > 0.66f) ? GREEN :
                (value > 0.33f) ? YELLOW :
                                  RED;

    DrawRectangle(x, y, width, height, (Color){25,25,35,200});

    float filledWidth = width * value;
    DrawRectangle(x, y, filledWidth, height, col);
}

void DrawSideHUDBig(float v1, float v2, float v3)
{
    float barWidth  = 140;
    float barHeight = 24;
    float spacing   = 15;

    float hudWidth  = barWidth * 3 + spacing * 2 + 40;
    float hudHeight = barHeight + 40;

    float hudX = GetScreenWidth()  - hudWidth  - 20;
    float hudY = GetScreenHeight() - hudHeight - 20;

    DrawRectangle(hudX, hudY, hudWidth, hudHeight, (Color){15,15,20,200});

    float startX = hudX + 20;
    float startY = hudY + (hudHeight - barHeight)/2;

    DrawHorizontalBar(startX, startY, barWidth, barHeight, v1);
    DrawHorizontalBar(startX + barWidth + spacing, startY, barWidth, barHeight, v2);
    DrawHorizontalBar(startX + 2*(barWidth + spacing), startY, barWidth, barHeight, v3);
}

void UpdateBars(int playerGold, int *prevGold) {

    if (playerGold > *prevGold) {
        *prevGold = playerGold;
    }

    int delta = *prevGold - playerGold;

    if (delta >= 10) {
        int steps = delta / 10;

        barsaude -= steps * 0.1f;
        barcomida -= steps * 0.1f;
        barinfra -= steps * 0.1f;
    }
    *prevGold = playerGold;
    printf("%f", barsaude);
}

void RankingHUD(int screenHeight)
{
    int size = 192;
    Vector2 pos = {10, screenHeight - size - 10};

    DrawTexturePro(
        cobreTexture,
        (Rectangle){0, 0, cobreTexture.width, cobreTexture.height},
        (Rectangle){pos.x, pos.y, size, size},
        (Vector2){0, 0},
        0,
        WHITE
    );
    float textX = pos.x + size + 10;
    float textY = pos.y + size/2 - 15;
    DrawText("NÍVEL 1", textX, textY, 30, WHITE);
}