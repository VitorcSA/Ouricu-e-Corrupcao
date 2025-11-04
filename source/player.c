#include <raylib.h>
#include "player.h"

#define MAX_TOWERS 50

typedef struct {
    Vector2 pos;
    Vector2 basePos;
    float size;
    bool active;
} Tower;

static Tower towers[MAX_TOWERS];
static int towerCount = 0;
static Texture2D torreTexture;

void InitPlayer() {
    Image torre = LoadImage("assets/torre_colocar.png");
    ImageResize(&torre, 64, 64);
    torreTexture = LoadTextureFromImage(torre);
    UnloadImage(torre);
}

void AddTower(Vector2 pos) {
    if (towerCount >= MAX_TOWERS) return;
    towers[towerCount].pos = pos;
    towers[towerCount].basePos = (Vector2){
        pos.x / ((float)GetScreenWidth() / 1280.0f),
        pos.y / ((float)GetScreenHeight() / 720.0f)
    };
    towers[towerCount].size = 64;
    towers[towerCount].active = true;
    towerCount++;
}

void UpdatePlayer() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();
        AddTower(mouse);
    }
}

void DrawTowers() {
    for (int i = 0; i < towerCount; i++) {
        if (towers[i].active)
            DrawTextureEx(torreTexture, (Vector2){towers[i].pos.x - towers[i].size / 2, towers[i].pos.y - towers[i].size / 2}, 0.0f, towers[i].size / 64.0f, WHITE);
    }
}

void RecenterTowers(int newWidth, int newHeight) {
    float scaleX = (float)newWidth / 1280.0f;
    float scaleY = (float)newHeight / 720.0f;

    for (int i = 0; i < towerCount; i++) {
        towers[i].pos.x = towers[i].basePos.x * scaleX;
        towers[i].pos.y = towers[i].basePos.y * scaleY;
        towers[i].size = 64 * scaleX;
    }
}

void UnloadPlayer() {
    UnloadTexture(torreTexture);
}