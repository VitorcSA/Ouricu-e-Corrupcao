#include "enemies.h"
#include "raylib.h"
#include <stdlib.h>
#include <math.h>
#include <raymath.h>

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

Vector2 GetNextTile(Vector2 current, unsigned char *map) {
    int y = (int)current.y;
    int x = (int)current.x;
    
    bool visitedCheck[ROWS][COLS] = { false };
    visitedCheck[y][x] = true;


    int directions[4][2] = {
        {0, -1}, // cima
        {0, 1},  // baixo
        {-1, 0}, // esquerda
        {1, 0}   // direita
    };

    for (int i = 0; i < 4; i++) {
        int ny = y + directions[i][1];
        int nx = x + directions[i][0];

        if (ny >= 0 && ny < ROWS && nx >= 0 && nx < COLS) {
            if (map[ny * COLS + nx] != 0 && !visitedCheck[ny][nx]) {
                return (Vector2){nx, ny};
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
        enemies2[i].frame = 0;
        enemies2[i].frameTime = 0;
    }
}

void SpawnEnemy(Vector2 start, unsigned char *map) {
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

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies2[i].active) {
            enemies2[i].active = true;
            enemies2[i].pos = start;
            enemies2[i].speed = 50 + GetRandomValue(0, 30);
            enemies2[i].target = GetNextTile(enemies2[i].pos, map);
            enemies2[i].frame = 0;
            enemies2[i].frameTime = 0;
            break;
        }
    }
}

void UpdateEnemy2(unsigned char *map, float tileWidth, float tileHeight, float delta) {
    for(int i = 0; i < MAX_ENEMIES; i++){
        if (!enemies2[i].active) continue;

    Vector2 currentPixel = { enemies2[i].pos.x * tileWidth + tileWidth/2, enemies2[i].pos.y * tileWidth + tileWidth/2 };
    Vector2 targetPixel  = { enemies2[i].target.x * tileWidth + tileWidth/2, enemies2[i].target.y * tileWidth + tileWidth/2 };

    Vector2 dir = Vector2Subtract(targetPixel, currentPixel);
    float dist = Vector2Length(dir);

    if (dist > 2.0f) {
        dir = Vector2Normalize(dir);
        currentPixel = Vector2Add(currentPixel, Vector2Scale(dir, enemies2[i].speed * delta));
    } else {
        enemies2[i].pos = enemies2[i].target;
        Vector2 next = GetNextTile(enemies2[i].pos, map);
        if (next.x >= 0) enemies2[i].target = next;
    }

    // Desenha o inimigo
    DrawCircleV(currentPixel, tileWidth * 0.3f, RED);
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