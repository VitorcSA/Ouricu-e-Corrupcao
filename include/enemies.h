#ifndef ENEMIES_H
#define ENEMIES_H

#include "raylib.h"

#define MAX_ENEMIES 50

typedef struct {
    Vector2 pos;
    float speed;
    float health;
    int active;
    float progress;
    int frame;
    float frameTime;
} Enemy;

extern Enemy enemies[MAX_ENEMIES];

void InitEnemies(void);
void SpawnEnemy(Vector2 pathStart);
void UpdateEnemies(float dt, Vector2 pathStart, Vector2 pathEnd);
void DrawEnemies(void);
void UnloadEnemies(void);

#endif