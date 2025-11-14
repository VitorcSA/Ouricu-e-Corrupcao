#ifndef ENEMIES_H
#define ENEMIES_H

#include "raylib.h"

#define MAX_ENEMIES 50
#define MAX_ORCS 50

#define ROWS 15
#define COLS 15

#define ARCHER_QT_FRAMES_SHOOT 14

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
void SpawnEnemy(unsigned char *map, float tileWidth, float tileHeight);
void SpawnOrcs(Vector2 pathStart);
void UpdateEnemy2(unsigned char *map, float tileWidth, float tileHeight, float delta);
void UpdateOrcs(float dt, Vector2 pathStart, Vector2 pathEnd);
void DrawEnemies2(void); 
void DrawOrcs(void);
void UnloadEnemies(void);
void UnloadOrcs(void);

#endif