#ifndef ENEMIES_H
#define ENEMIES_H

#include "raylib.h"

#define MAX_ENEMIES 50
#define MAX_ORCS 50

#define ROWS 15
#define COLS 15

#define ENEMY_QT_FRAMES 8

#define MAX_NORMAL_ENEMIES_HEALTH 30.0f

typedef struct {
    Vector2 pos;        // posição atual em tiles
    Vector2 pixelPos;   // posição real (em pixels)
    Vector2 target;     // próximo tile
    Vector2 lastTarget;
    float health;
    float currentHealth;
    float speed;
    float size;
    float frameTime;
    int frame;
    int active;
} Enemy;

extern Texture2D walkTexture;

extern Enemy enemies[MAX_ENEMIES];
extern Enemy orcs[MAX_ORCS];

extern int vidaPortao;

void DrawEnemies2(Enemy *enemy, Texture2D enemyTexture, int maxEnemies);
void InitEnemiesTexture(void);
void InitEnemy(Enemy *enemy, float health, int maxEnemys);
void recenterEnemies(int newWidth, int newHeight);
void SpawnEnemy(Enemy *enemy, unsigned char *map, float tileWidth, float tileHeight, int i);
void SpawnOrcs(Vector2 pathStart);
void UpdateEnemy2(Enemy *enemy, unsigned char *map, float tileWidth, float tileHeight, float delta, int *vidaPortao);
void UpdateOrcs(float dt, Vector2 pathStart, Vector2 pathEnd);
void UnloadEnemies(void);
void ResetEnemies(Enemy *enemies, int maxEnemies);

bool TodosInimigosMortos(Enemy *enemies, int maxEnemies);
    
#endif