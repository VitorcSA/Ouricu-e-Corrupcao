#include "enemies.h"
#include "raylib.h"
#include <stdlib.h>
#include <math.h>

Enemy enemies[MAX_ENEMIES];

static float Lerp(float a, float b, float t) {
    return a + t * (b - a);
}
static float Vector2Distance(Vector2 a, Vector2 b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return sqrtf(dx * dx + dy * dy);
}
void InitEnemies() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = false;
        enemies[i].progress = 0;
    }
}

void SpawnEnemy(Vector2 start) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].active = true;
            enemies[i].pos = start;
            enemies[i].speed = 50 + GetRandomValue(0, 30);
            enemies[i].progress = 0;
            break;
        }
    }
}

void UpdateEnemies(float dt, Vector2 pathStart, Vector2 pathEnd) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            enemies[i].progress += dt * (enemies[i].speed / Vector2Distance(pathStart, pathEnd));
            if (enemies[i].progress >= 1.0f) {
                enemies[i].active = false;
                continue;
            }
            enemies[i].pos.x = Lerp(pathStart.x, pathEnd.x, enemies[i].progress);
            enemies[i].pos.y = Lerp(pathStart.y, pathEnd.y, enemies[i].progress);
        }
    }
}

void DrawEnemies() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            DrawCircleV(enemies[i].pos, 10, RED);
        }
    }
}