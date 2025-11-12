#ifndef ENEMIES_H
#define ENEMIES_H

#include "raylib.h"

#define MAX_ENEMIES 50
#define MAX_ORCS 50

typedef struct {
    Vector2 pos;
    float speed;
    float health;
    int active;
    float progress;
    int frame;
    float frameTime;
} Enemy;

typedef struct {
    Vector2 pos;
    float speed;
    float health;
    int active;
    float progress;
    int frame;
    float frameTime;
} Orc;

extern Enemy enemies[MAX_ENEMIES];
extern Orc orcs[MAX_ORCS];

void InitEnemies(void);
void InitOrcs(void);
void SpawnEnemy(Vector2 pathStart);
void SpawnOrcs(Vector2 pathStart);
void UpdateEnemies(float dt, Vector2 pathStart, Vector2 pathEnd);
void UpdateOrcs(float dt, Vector2 pathStart, Vector2 pathEnd);
void DrawEnemies(void);
void DrawOrcs(void);
void UnloadEnemies(void);
void UnloadOrcs(void);

#endif