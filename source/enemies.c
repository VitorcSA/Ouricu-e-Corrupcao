#include "enemies.h"
#include "raylib.h"
#include <stdlib.h>
#include <math.h>

Enemy enemies[MAX_ENEMIES];
Orc orcs[MAX_ORCS];

static Texture2D walkTexture;
static Texture2D OrcTexture;
static Rectangle frameRec;
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

    frameWidth = (float)walkTexture.width / 8;
    frameHeight = (float)walkTexture.height;
    frameRec = (Rectangle){0, 0, frameWidth, frameHeight};

    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = false;
        enemies[i].progress = 0;
        enemies[i].frame = 0;
        enemies[i].frameTime = 0;
    }
}

void InitOrcs() {
    Image OrcImage = LoadImage("assets/Orc.png");
    OrcTexture = LoadTextureFromImage(OrcImage);
    UnloadImage(OrcImage);

    frameWidth = (float)OrcTexture.width / 7;
    frameHeight = (float)OrcTexture.height;
    frameRec = (Rectangle){0, 0, frameWidth, frameHeight};

    for (int i = 0; i < MAX_ORCS; i++) {
        orcs[i].active = false;
        orcs[i].progress = 0;
        orcs[i].frame = 0;
        orcs[i].frameTime = 0;
    }
}

void SpawnEnemy(Vector2 start) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].active = true;
            enemies[i].pos = start;
            enemies[i].speed = 50;
            enemies[i].progress = 0;
            enemies[i].frame = 0;
            enemies[i].frameTime = 0;
            enemies[i].health = 10;
            break;
        }
    }
}

void SpawnOrcs(Vector2 start) {
    for (int i = 0; i < MAX_ORCS; i++) {
        if (!orcs[i].active) {
            orcs[i].active = true;
            orcs[i].pos = start;
            orcs[i].speed = 25;
            orcs[i].progress = 0;
            orcs[i].frame = 0;
            orcs[i].frameTime = 0;
            orcs[i].health = 50;
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
            enemies[i].frame = (enemies[i].frame + 1) % 8;
        }
    }
}

void UpdateOrcs(float dt, Vector2 pathStart, Vector2 pathEnd) {
    for (int i = 0; i < MAX_ORCS; i++) {
        if (!orcs[i].active) continue;

        orcs[i].progress += dt * (orcs[i].speed / Vector2Distance(pathStart, pathEnd));
        if (orcs[i].progress >= 1.0f) {
            orcs[i].active = false;
            continue;
        }

        orcs[i].pos.x = Lerp(pathStart.x, pathEnd.x, orcs[i].progress);
        orcs[i].pos.y = Lerp(pathStart.y, pathEnd.y, orcs[i].progress);

        orcs[i].frameTime += dt;
        if (orcs[i].frameTime >= 0.1f) {
            orcs[i].frameTime = 0;
            orcs[i].frame = (orcs[i].frame + 1) % 7;
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

void DrawOrcs() {
    for (int i = 0; i < MAX_ORCS; i++) {
        if (!orcs[i].active) continue;

        Rectangle src = { orcs[i].frame * frameWidth, 0, frameWidth, frameHeight };
        Vector2 pos = { orcs[i].pos.x - frameWidth / 2, orcs[i].pos.y - frameHeight / 2 };
        DrawTextureRec(OrcTexture, src, pos, WHITE);
    }
}

void UnloadOrcs() {
    UnloadTexture(OrcTexture);
}