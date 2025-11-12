#include "enemies.h"
#include "raylib.h"
#include <stdlib.h>
#include <math.h>
#include <raymath.h>
#include <stdio.h>

Enemy enemies[MAX_ENEMIES];

static Texture2D walkTexture;
static const int totalFrames = 8;
static float frameWidth, frameHeight;

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

void InitEnemies() {
    Image img = LoadImage("assets/Walk.png");
    walkTexture = LoadTextureFromImage(img);
    UnloadImage(img);
    

    frameWidth = (float)walkTexture.width / totalFrames;
    frameHeight = (float)walkTexture.height;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = false;
        enemies[i].pos = (Vector2){0, 0};
        enemies[i].target = (Vector2){0, 0};
        enemies[i].lastTarget = (Vector2){0, 0};
        enemies[i].speed = 0;
        enemies[i].frame = 0;
        enemies[i].frameTime = 0;

    }
}

void SpawnEnemy(unsigned char *map, float tileWidth, float tileHeight) {
    Vector2 startTile = FindStart(map);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].active = true;
            enemies[i].pos = startTile;
            enemies[i].pixelPos = (Vector2){ startTile.x * tileWidth + tileWidth/2,
                                              startTile.y * tileHeight + tileHeight/2 };
            enemies[i].target = GetNextTile(startTile, startTile, map);
            enemies[i].lastTarget = startTile;
            enemies[i].speed = 60;
            enemies[i].frame = 0;
            enemies[i].frameTime = 0;
            break;
        }
    }
}

void UpdateEnemy2(unsigned char *map, float tileWidth, float tileHeight, float delta) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        Vector2 targetPixel = {
            enemies[i].target.x * tileWidth + tileWidth / 2,
            enemies[i].target.y * tileHeight + tileHeight / 2
        };

        Vector2 dir = Vector2Subtract(targetPixel, enemies[i].pixelPos);
        float dist = Vector2Length(dir);

        // se ainda não chegou ao próximo tile
        if (dist > 2.0f) {
            dir = Vector2Normalize(dir);
            enemies[i].pixelPos = Vector2Add(enemies[i].pixelPos, Vector2Scale(dir, enemies[i].speed * delta));
        } else {
            // chegou no tile alvo — atualiza o tile atual e busca o próximo
            enemies[i].lastTarget = (Vector2)enemies[i].pos;;
            enemies[i].pos = enemies[i].target;
            Vector2 next = GetNextTile(enemies[i].pos, enemies[i].lastTarget, map);

            // se não há próximo, desativa o inimigo (chegou ao fim)
            if (next.x == enemies[i].pos.x && next.y == enemies[i].pos.y) {
                enemies[i].active = false;
                continue;
            }

            enemies[i].target = next;
        }
    }
}

void DrawEnemies2() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        Rectangle src = { enemies[i].frame * frameWidth, 0, frameWidth, frameHeight };
        Vector2 pos = { enemies[i].pixelPos.x - frameWidth / 2,
                        enemies[i].pixelPos.y - frameHeight / 2 };
        DrawTextureRec(walkTexture, src, pos, WHITE);
    }
}

void UnloadEnemies() {
    UnloadTexture(walkTexture);
}