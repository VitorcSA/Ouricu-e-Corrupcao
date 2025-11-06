#include "enemies.h"
#include "raylib.h"
#include <stdlib.h>
#include <math.h>

Enemy enemies[MAX_ENEMIES];

static Texture2D walkTexture;
static Rectangle frameRec;
static const int totalFrames = 8;
static float frameWidth, frameHeight;

static float Lerp(float a, float b, float t) {
    return a + t * (b - a);
}

static float Vector2Distance(Vector2 a, Vector2 b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return sqrtf(dx * dx + dy * dy);
}

void InitEnemies() {
    Image img = LoadImage("assets/Walk.png");
    walkTexture = LoadTextureFromImage(img);
    UnloadImage(img);

    frameWidth = (float)walkTexture.width / totalFrames;
    frameHeight = (float)walkTexture.height;
    frameRec = (Rectangle){0, 0, frameWidth, frameHeight};

    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = false;
        enemies[i].progress = 0;
        enemies[i].frame = 0;
        enemies[i].frameTime = 0;
    }
}

void SpawnEnemy(Vector2 start) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].active = true;
            enemies[i].pos = start;
            enemies[i].speed = 50 + GetRandomValue(0, 30);
            enemies[i].progress = 0;
            enemies[i].frame = 0;
            enemies[i].frameTime = 0;
            break;
        }
    }
}

void UpdateEnemies(float dt, Vector2 pathStart, Vector2 pathEnd) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        enemies[i].progress += dt * (enemies[i].speed / Vector2Distance(pathStart, pathEnd));
        if (enemies[i].progress >= 1.0f) {
            enemies[i].active = false;
            continue;
        }

        enemies[i].pos.x = Lerp(pathStart.x, pathEnd.x, enemies[i].progress);
        enemies[i].pos.y = Lerp(pathStart.y, pathEnd.y, enemies[i].progress);

        enemies[i].frameTime += dt;
        if (enemies[i].frameTime >= 0.1f) {
            enemies[i].frameTime = 0;
            enemies[i].frame = (enemies[i].frame + 1) % totalFrames;
        }
    }
}

void DrawEnemies() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        Rectangle src = { enemies[i].frame * frameWidth, 0, frameWidth, frameHeight };
        Vector2 pos = { enemies[i].pos.x - frameWidth / 2, enemies[i].pos.y - frameHeight / 2 };
        DrawTextureRec(walkTexture, src, pos, WHITE);
    }
}

void UnloadEnemies() {
    UnloadTexture(walkTexture);
}