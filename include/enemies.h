#ifndef ENEMIES_H
#define ENEMIES_H

#include "raylib.h"

#define MAX_ENEMIES 50
#define MAX_GOLEMS 50

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
} Golem;

extern Enemy enemies[MAX_ENEMIES];
extern Golem golems[MAX_GOLEMS];

void InitEnemies(void);
void InitGolems(void);
void SpawnEnemy(Vector2 pathStart);
void SpawnGolems(Vector2 pathStart);
void UpdateEnemies(float dt, Vector2 pathStart, Vector2 pathEnd);
void UpdateGolems(float dt, Vector2 pathStart, Vector2 pathEnd);
void DrawEnemies(void);
void DrawGolems(void);
void UnloadEnemies(void);
void UnloadGolems(void);

#endif