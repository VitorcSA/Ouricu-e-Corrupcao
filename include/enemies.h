#ifndef ENEMIES_H
#define ENEMIES_H

#include "raylib.h"

#define MAX_ENEMIES 50
#define ROWS 15
#define COLS 15

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
    Vector2 pos;        // posição atual em tiles
    Vector2 pixelPos;   // posição real (em pixels)
    Vector2 target;     // próximo tile
    Vector2 lastTarget;
    bool active;
    float speed;
    int frame;
    float frameTime;
} Enemy2;

extern Enemy enemies[MAX_ENEMIES];
extern Enemy2 enemy2[MAX_ENEMIES];

void InitEnemies(unsigned char *map);
void SpawnEnemy(Vector2 start, unsigned char *map, float tileWidth, float tileHeight);
void UpdateEnemies(float dt, Vector2 pathStart, Vector2 pathEnd);
void UpdateEnemy2(unsigned char *map, float tileWidth, float tileHeight, float delta);
void DrawEnemies(void);
void DrawEnemies2(void); 
void UnloadEnemies(void);

#endif