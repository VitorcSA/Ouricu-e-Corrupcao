#ifndef ENEMIES_H
#define ENEMIES_H

#include "raylib.h"

#define MAX_ENEMIES 50
#define ROWS 15
#define COLS 15

typedef struct {
    Vector2 pos;        // posição atual em tiles
    Vector2 pixelPos;   // posição real (em pixels)
    Vector2 target;     // próximo tile
    Vector2 lastTarget;
    int active;
    float health;
    float speed;
    int frame;
    float frameTime;
} Enemy;

extern Enemy enemies[MAX_ENEMIES];

void InitEnemies(void);
void SpawnEnemy(unsigned char *map, float tileWidth, float tileHeight);
void UpdateEnemy2(unsigned char *map, float tileWidth, float tileHeight, float delta);
void DrawEnemies2(void); 
void UnloadEnemies(void);

#endif