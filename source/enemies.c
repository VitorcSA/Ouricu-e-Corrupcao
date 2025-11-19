#include "enemies.h"
#include "raylib.h"
#include <stdlib.h>
#include <math.h>
#include <raymath.h>
#include <stdio.h>

Enemy enemies[MAX_ENEMIES];
Enemy orcs[MAX_ORCS];

Texture2D walkTexture;
Texture2D OrcTexture;

int vidaPortao;

Vector2 FindStart(unsigned char *map) {
    for (int x = 0; x < ROWS; x++) {
        for (int y = 0; y < COLS; y++) {
            if (map[y * COLS + x] != 0) {
                return (Vector2){x, y};
            }
        }
    }
    return (Vector2){0,0};
}

Vector2 GetNextTile(Vector2 current, Vector2 last, unsigned char *map) {
    int y = (int)current.y;
    int x = (int)current.x;

    int lstY = (int)last.y;
    int lstX = (int)last.x;

    int directions[4][2] = {
        {1, 0},   // direita
        {0, -1}, // cima
        {0, 1},  // baixo
        {-1, 0} // esquerda
    };

    for (int i = 0; i < 4; i++) {
        int ny = y + directions[i][1];
        int nx = x + directions[i][0];

        if (ny >= 0 && ny < ROWS && nx >= 0 && nx < COLS) {
            if (map[ny * COLS + nx] != 0 && !(nx == (int)last.x && ny == (int)last.y)) {
                return (Vector2){nx , ny };
            }
        }
    }

    // se não encontrar, retorna o próprio
    return current;
}

void InitEnemiesTexture(){
    Image img = LoadImage("assets/Walk.png");
    walkTexture = LoadTextureFromImage(img);
    UnloadImage(img);

    Image OrcImage = LoadImage("assets/Orc.png");
    OrcTexture = LoadTextureFromImage(OrcImage);
    UnloadImage(OrcImage);
}

void InitEnemy(Enemy *enemy, float health, int maxEnemys){
    float cellWidth  = (float)GetScreenWidth() / (float)COLS;
    float cellHeight = (float)GetScreenHeight() / (float)ROWS;

    float s = (cellWidth < cellHeight) ? cellWidth : cellHeight;

    for (int i = 0; i < maxEnemys; i++) {
        enemy[i].active = false;
        enemy[i].pos = (Vector2){0, 0};
        enemy[i].target = (Vector2){0, 0};
        enemy[i].lastTarget = (Vector2){0, 0};
        enemy[i].speed = 0;
        enemy[i].frame = 0;
        enemy[i].frameTime = 0;
        enemy[i].health = health;
        enemy[i].size = s / 64.0f;
    }
}

void SpawnEnemy(Enemy *enemy, unsigned char *map, float tileWidth, float tileHeight) {
    Vector2 startTile = FindStart(map);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemy[i].active) {
            enemy[i].active = true;
            enemy[i].pos = startTile;
            enemy[i].pixelPos = (Vector2){ startTile.x * tileWidth + tileWidth/2,
                                              startTile.y * tileHeight + tileHeight/2 };
            enemy[i].target = GetNextTile(startTile, startTile, map);
            enemy[i].lastTarget = startTile;
            enemy[i].speed = 2.0f; // inimigo anda 3 tiles por segundo
            enemy[i].frame = 0;
            enemy[i].frameTime = 0;
            enemy[i].health = 10;
            break;
        }
    }
}

void UpdateEnemy2(Enemy *enemy, unsigned char *map, float tileWidth, float tileHeight, float delta, int *vidaPortao) {
    float tileSize = (tileWidth < tileHeight) ? tileWidth : tileHeight;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemy[i].active) continue;

        enemy[i].frameTime += delta;
        if (enemy[i].frameTime >= 0.1f) {
            enemy[i].frame = (enemy[i].frame + 1) % ENEMY_QT_FRAMES;
            enemy[i].frameTime = 0;
        }

        Vector2 targetPixel = {
            enemy[i].target.x * tileWidth + tileWidth / 2,
            enemy[i].target.y * tileHeight + tileHeight / 2
        };

        Vector2 dir = Vector2Subtract(targetPixel, enemy[i].pixelPos);
        float dist = Vector2Length(dir);
        float speedPx = enemy[i].speed * tileSize;

        // se ainda não chegou ao próximo tile
        if (dist > 2.0f) {
            dir = Vector2Normalize(dir);
            enemy[i].pixelPos = Vector2Add(enemy[i].pixelPos, Vector2Scale(dir, speedPx * delta));
        } else {
            // chegou no tile alvo — atualiza o tile atual e busca o próximo
            enemy[i].lastTarget = (Vector2)enemy[i].pos;;
            enemy[i].pos = enemy[i].target;

            enemy[i].pixelPos.x = enemy[i].pos.x * tileWidth  + tileWidth/2;
            enemy[i].pixelPos.y = enemy[i].pos.y * tileHeight + tileHeight/2;

            Vector2 next = GetNextTile(enemy[i].pos, enemy[i].lastTarget, map);

            int gridX = enemy[i].pos.x;
            int gridY = enemy[i].pos.y;

            // se não há próximo, desativa o inimigo (chegou ao fim)
            if ((next.x == enemy[i].pos.x && next.y == enemy[i].pos.y) || map[gridY * COLS + gridX] >= 8) {
                *vidaPortao -= 1;
                printf("%d\n", *vidaPortao);
                enemy[i].active = false;
                continue;
            }

            enemy[i].target = next;
        }
    }
}

void DrawEnemies2(Enemy *enemy, Texture2D enemyTexture, int maxEnemies) {
    float frameWidth = (float)enemyTexture.width / ENEMY_QT_FRAMES;
    float frameHeight = (float)enemyTexture.height;

    for(int i = 0; i < maxEnemies; i++){
        if (!enemy[i].active) continue;

        float w = frameWidth  * enemy[i].size;
        float h = frameHeight * enemy[i].size;

        float offset = h / 6.0f;

        Rectangle src = (Rectangle){  frameWidth * enemy[i].frame, 
                            0, 
                            frameWidth, 
                            enemyTexture.height };

        Rectangle dest = (Rectangle){ enemy[i].pixelPos.x, 
                            enemy[i].pixelPos.y, 
                            w, 
                            h };
        
        Vector2 origin = { w /4, h  };  // mantém centro fixo

        DrawTexturePro( enemyTexture, 
                        src, 
                        dest, 
                        origin, 
                        0.0f, 
                        WHITE);  
    }
}
void recenterEnemy(Enemy *enemy, int newWidth, int newHeight, int maxEnemies){
    float scaleX = (float)newWidth;
    float scaleY = (float)newHeight;

    float cellWidth = newWidth / (float)COLS;
    float cellHeight = newHeight / (float)ROWS;
    float s = (cellWidth < cellHeight) ? cellWidth : cellHeight;

    for (int i = 0; i < maxEnemies; i++) {
        enemies[i].pixelPos.x = enemies[i].pos.x * cellWidth + cellWidth / 2;
        enemies[i].pixelPos.y = enemies[i].pos.y * cellHeight + cellWidth / 2;
        enemies[i].size = s * 0.01562f;
    }
}

void recenterEnemies(int newWidth, int newHeight){
    recenterEnemy(enemies, newWidth, newHeight, MAX_ENEMIES);
}

void UnloadEnemies() {
    UnloadTexture(walkTexture);
    UnloadTexture(OrcTexture);
}