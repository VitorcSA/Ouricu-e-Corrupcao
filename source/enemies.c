#include "enemies.h"
#include "raylib.h"
#include <stdlib.h>
#include <math.h>
#include <raymath.h>
#include <stdio.h>

Enemy enemies[MAX_ENEMIES];
Enemy2 enemies2[MAX_ENEMIES];

static Texture2D walkTexture;
static Rectangle frameRec;
static const int totalFrames = 8;
static float frameWidth, frameHeight;

static float Lerpfunc(float a, float b, float t) {
    return a + t * (b - a);
}

static float Vector2DistanceFunc(Vector2 a, Vector2 b) {
    float dx = b.x - a.x;
    float dy = b.y - a.y;
    return sqrtf(dx * dx + dy * dy);
}

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

        printf("fora do if");
        if (ny >= 0 && ny < ROWS && nx >= 0 && nx < COLS) {
            if (map[ny * COLS + nx] != 0 && !(nx == (int)last.x && ny == (int)last.y)) {
                printf("dentro Do if\n");
                printf("X current %d\n", x);
                printf("X last %d\n", lstX);
                printf("Y Current%d\n", y);
                printf("Y last %d\n", lstY);
                return (Vector2){nx , ny };
            }
        }
    }

    // se não encontrar, retorna o próprio
    return current;
}

void InitEnemies(unsigned char *map) {
    Image img = LoadImage("assets/Walk.png");
    walkTexture = LoadTextureFromImage(img);
    UnloadImage(img);
    

    frameWidth = (float)walkTexture.width / totalFrames;
    frameHeight = (float)walkTexture.height;
    frameRec = (Rectangle){0, 0, frameWidth, frameHeight};

    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i].active = false;
        enemies[i].progress = 0;
        enemies[i].frame = 0;
        enemies[i].frameTime = 0;
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies2[i].active = false;
        enemies2[i].pos = (Vector2){0, 0};
        enemies2[i].target = (Vector2){0, 0};
        enemies2[i].lastTarget = (Vector2){0, 0};
        enemies2[i].speed = 0;
        enemies2[i].frame = 0;
        enemies2[i].frameTime = 0;

    }
}

void SpawnEnemy(Vector2 start, unsigned char *map, float tileWidth, float tileHeight) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) {
            enemies[i].active = true;
            enemies[i].pos = start;
            enemies[i].speed = 50 + GetRandomValue(0, 30);
            enemies[i].progress = 0;
            enemies[i].frame = 0;
            enemies[i].frameTime = 0;
            break;
        }
    }

Vector2 startTile = FindStart(map);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies2[i].active) {
            enemies2[i].active = true;
            enemies2[i].pos = startTile;
            enemies2[i].pixelPos = (Vector2){ startTile.x * tileWidth + tileWidth/2,
                                              startTile.y * tileHeight + tileHeight/2 };
            enemies2[i].target = GetNextTile(startTile, startTile, map);
            enemies2[i].lastTarget = startTile;
            enemies2[i].speed = 60;
            enemies2[i].frame = 0;
            enemies2[i].frameTime = 0;
            break;
        }
    }
}

void UpdateEnemy2(unsigned char *map, float tileWidth, float tileHeight, float delta) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies2[i].active) continue;

        Vector2 targetPixel = {
            enemies2[i].target.x * tileWidth + tileWidth / 2,
            enemies2[i].target.y * tileHeight + tileHeight / 2
        };

        Vector2 dir = Vector2Subtract(targetPixel, enemies2[i].pixelPos);
        float dist = Vector2Length(dir);

        // se ainda não chegou ao próximo tile
        if (dist > 2.0f) {
            dir = Vector2Normalize(dir);
            enemies2[i].pixelPos = Vector2Add(enemies2[i].pixelPos, Vector2Scale(dir, enemies2[i].speed * delta));
        } else {
            printf("testeeeeeeee\n");
            // chegou no tile alvo — atualiza o tile atual e busca o próximo
            enemies2[i].lastTarget = (Vector2)enemies2[i].pos;
            printf("Enemie %d last X: %d\n", i, (int)enemies2[i].lastTarget.x);
            printf("Enemie %d last Y: %d\n", i, (int)enemies2[i].lastTarget.y);
            enemies2[i].pos = enemies2[i].target;
            printf("Enemie %d pos X: %d\n", i, (int)enemies2[i].pos.x);
            printf("Enemie %d pos Y: %d\n", i, (int)enemies2[i].pos.y);
            Vector2 next = GetNextTile(enemies2[i].pos, enemies2[i].lastTarget, map);

            // se não há próximo, desativa o inimigo (chegou ao fim)
            if (next.x == enemies2[i].pos.x && next.y == enemies2[i].pos.y) {
                enemies2[i].active = false;
                continue;
            }

            enemies2[i].target = next;
        }
    }
}


void DrawEnemies2() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies2[i].active) continue;

        Rectangle src = { enemies2[i].frame * frameWidth, 0, frameWidth, frameHeight };
        Vector2 pos = { enemies2[i].pixelPos.x - frameWidth / 2,
                        enemies2[i].pixelPos.y - frameHeight / 2 };
        DrawTextureRec(walkTexture, src, pos, WHITE);
    }
}


void UpdateEnemies(float dt, Vector2 pathStart, Vector2 pathEnd) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        enemies[i].progress += dt * (enemies[i].speed / Vector2DistanceFunc(pathStart, pathEnd));
        if (enemies[i].progress >= 1.0f) {
            enemies[i].active = false;
            continue;
        }

        enemies[i].pos.x = Lerpfunc(pathStart.x, pathEnd.x, enemies[i].progress);
        enemies[i].pos.y = Lerpfunc(pathStart.y, pathEnd.y, enemies[i].progress);

        enemies[i].frameTime += dt;
        if (enemies[i].frameTime >= 0.1f) {
            enemies[i].frameTime = 0;
            enemies[i].frame = (enemies[i].frame + 1) % totalFrames;
        }
    }
}

void DrawEnemies() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].active) continue;

        Rectangle src = { enemies[i].frame * frameWidth, 0, frameWidth, frameHeight };
        Vector2 pos = { enemies[i].pos.x - frameWidth / 2, enemies[i].pos.y - frameHeight / 2 };
        DrawTextureRec(walkTexture, src, pos, WHITE);
    }
}

void UnloadEnemies() {
    UnloadTexture(walkTexture);
}