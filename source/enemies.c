#include "enemies.h"
#include "raylib.h"
#include <stdlib.h>
#include <math.h>

Enemy enemies[MAX_ENEMIES];
Golem golems[MAX_GOLEMS];

static Texture2D walkTexture;
static Texture2D GolemTexture;
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

void InitGolems() {
    Image GolemImage = LoadImage("assets/Golem.png");
    GolemTexture = LoadTextureFromImage(GolemImage);
    UnloadImage(GolemImage);

    frameWidth = (float)GolemTexture.width / 11;
    frameHeight = (float)GolemTexture.height;
    frameRec = (Rectangle){0, 0, frameWidth, frameHeight};

    for (int i = 0; i < MAX_GOLEMS; i++) {
        golems[i].active = false;
        golems[i].progress = 0;
        golems[i].frame = 0;
        golems[i].frameTime = 0;
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
            break;
        }
    }
}

void SpawnGolems(Vector2 start) {
    for (int i = 0; i < MAX_GOLEMS; i++) {
        if (!golems[i].active) {
            golems[i].active = true;
            golems[i].pos = start;
            golems[i].speed = 25;
            golems[i].progress = 0;
            golems[i].frame = 0;
            golems[i].frameTime = 0;
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

void UpdateGolems(float dt, Vector2 pathStart, Vector2 pathEnd) {
    for (int i = 0; i < MAX_GOLEMS; i++) {
        if (!golems[i].active) continue;

        golems[i].progress += dt * (golems[i].speed / Vector2Distance(pathStart, pathEnd));
        if (golems[i].progress >= 1.0f) {
            golems[i].active = false;
            continue;
        }

        golems[i].pos.x = Lerp(pathStart.x, pathEnd.x, golems[i].progress);
        golems[i].pos.y = Lerp(pathStart.y, pathEnd.y, golems[i].progress);

        golems[i].frameTime += dt;
        if (golems[i].frameTime >= 0.1f) {
            golems[i].frameTime = 0;
            golems[i].frame = (golems[i].frame + 1) % 11;
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

void DrawGolems() {
    for (int i = 0; i < MAX_GOLEMS; i++) {
        if (!golems[i].active) continue;

        Rectangle src = { golems[i].frame * frameWidth, 0, frameWidth, frameHeight };
        Vector2 pos = { golems[i].pos.x - frameWidth / 2, golems[i].pos.y - frameHeight / 2 };
        DrawTextureRec(GolemTexture, src, pos, WHITE);
    }
}

void UnloadGolems() {
    UnloadTexture(GolemTexture);
}