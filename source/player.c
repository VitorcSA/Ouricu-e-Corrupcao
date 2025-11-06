#include <raylib.h>
#include "player.h"
#include "enemies.h"
#include <math.h>
#include <raymath.h>

#define MAX_TOWERS 50
#define MAX_ARCHERS 50
#define ATTACK_RANGE 225.0f
#define MAX_ARROWS 200

typedef struct {
    Vector2 pos;
    Vector2 basePos;
    float size;
    bool active;
} Tower;

typedef struct {
    Vector2 pos;
    bool active;
    int frame;
    float frameTime;
    float shotTimer;
    bool isShooting;
} Archer;

typedef struct {
    Vector2 pos;
    Vector2 target;
    bool active;
    float speed;
    int enemyIndex; // índice do inimigo alvo
} Arrow;

static Arrow arrows[MAX_ARROWS];
static Tower towers[MAX_TOWERS];
static Archer archers[MAX_ARCHERS];
static int towerCount = 0;
static int archerCount = 0;

static Texture2D torreTexture;
static Texture2D archerTexture;
static Texture2D archerIdeleTexture;
static Texture2D arrowTexture;

void InitPlayer() {
    Image torre = LoadImage("assets/torre_colocar.png");
    ImageResize(&torre, 64, 64);
    torreTexture = LoadTextureFromImage(torre);
    UnloadImage(torre);

    Image arqueiro = LoadImage("assets/inimigosAnimation/Shot.png");
    archerTexture = LoadTextureFromImage(arqueiro);
    UnloadImage(arqueiro);

    Image arqueiroIdele = LoadImage("assets/inimigosAnimation/Idle.png");
    archerIdeleTexture = LoadTextureFromImage(arqueiroIdele);
    UnloadImage(arqueiroIdele);

    Image arrow = LoadImage("assets/inimigosAnimation/Arrow.png");
    arrowTexture = LoadTextureFromImage(arrow);
    UnloadImage(arrow);
}

void AddTower(Vector2 pos) {
    if (towerCount >= MAX_TOWERS) return;
    towers[towerCount].pos = pos;
    towers[towerCount].basePos = (Vector2){
        pos.x / ((float)GetScreenWidth() / 1280.0f),
        pos.y / ((float)GetScreenHeight() / 720.0f)
    };
    towers[towerCount].size = 64;
    towers[towerCount].active = true;
    towerCount++;
}

void AddArcher(Vector2 pos) {
    if (archerCount >= MAX_ARCHERS) return;
    archers[archerCount].pos = pos;
    archers[archerCount].active = true;
    archers[archerCount].frame = 0;
    archers[archerCount].frameTime = 0;
    archers[archerCount].shotTimer = 0;
    archers[archerCount].isShooting = false;
    archerCount++;
}

void ShootArrow(Vector2 start, Vector2 target, int enemyIndex) {
    for (int i = 0; i < MAX_ARROWS; i++) {
        if (!arrows[i].active) {
            arrows[i].pos = start;
            arrows[i].target = target;
            arrows[i].speed = 600.0f;  // velocidade da flecha
            arrows[i].enemyIndex = enemyIndex;
            arrows[i].active = true;
            break;
        }
    }
}

void UpdateArrows(float dt) {
    for (int i = 0; i < MAX_ARROWS; i++) {
        if (!arrows[i].active) continue;

        Vector2 dir = Vector2Normalize(Vector2Subtract(arrows[i].target, arrows[i].pos));
        arrows[i].pos = Vector2Add(arrows[i].pos, Vector2Scale(dir, arrows[i].speed * dt));

        // aplica dano quando a flecha chega
        if (Vector2Distance(arrows[i].pos, arrows[i].target) < 5.0f) {
            int e = arrows[i].enemyIndex;
            if (e >= 0 && e < MAX_ENEMIES && enemies[e].active) {
                enemies[e].health -= 10;
                if (enemies[e].health <= 0) enemies[e].active = false;
            }
            arrows[i].active = false;
        }
    }
}

void UpdatePlayer(void) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();
        AddTower(mouse);
    }

    // Colocar arqueiro com botão direito em uma torre
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Vector2 mouse = GetMousePosition();
        for (int i = 0; i < towerCount; i++) {
            if (towers[i].active &&
                CheckCollisionPointCircle(mouse, towers[i].pos, towers[i].size / 2)) {
                AddArcher((Vector2){towers[i].pos.x, towers[i].pos.y - 32});
                break;
            }
        }
    }

    float dt = GetFrameTime();

    // Atualiza arqueiros
    for (int i = 0; i < archerCount; i++) {
        if (!archers[i].active) continue;

        // animação do arqueiro
        archers[i].frameTime += dt;
        if (archers[i].frameTime >= 0.1f) {
            archers[i].frame = (archers[i].frame + 1) % 14; // 14 frames no shot.png
            archers[i].frameTime = 0;
        }

        // controle de tiro
        archers[i].shotTimer -= dt;
        if (archers[i].shotTimer <= 0) {
            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (!enemies[e].active) continue;

                float dx = enemies[e].pos.x - archers[i].pos.x;
                float dy = enemies[e].pos.y - archers[i].pos.y;
                float dist = sqrtf(dx * dx + dy * dy);

                if (dist <= ATTACK_RANGE) {
                    archers[i].isShooting = true;
                    ShootArrow(archers[i].pos, enemies[e].pos, e);
                    archers[i].shotTimer = 1.0f; // atira a cada 1s
                    break;
                } else {
                    archers[i].isShooting = false;
                }
            }
        }
    }

    // Atualiza flechas
    UpdateArrows(dt);
}

void DrawTowers() {
    for (int i = 0; i < towerCount; i++) {
        if (towers[i].active)
            DrawTextureEx(torreTexture,
                (Vector2){towers[i].pos.x - towers[i].size / 2, towers[i].pos.y - towers[i].size / 2},
                0.0f, towers[i].size / 64.0f, WHITE);
    }
}

void DrawArchers() {
    int frameWidth = archerTexture.width / 14;
    int frameIdleWidth = archerIdeleTexture.width / 9;

    for (int i = 0; i < archerCount; i++) {
        if (!archers[i].active) continue;

        if (archers[i].isShooting) {
            Rectangle src = { frameWidth * archers[i].frame, 0, frameWidth, archerTexture.height };
            Rectangle dest = { archers[i].pos.x, archers[i].pos.y - 32, frameWidth, archerTexture.height };
            DrawTexturePro(archerTexture, src, dest,
                (Vector2){frameWidth / 2, archerTexture.height / 2}, 0.0f, WHITE);
        } else {
            Rectangle srcIdel = { frameIdleWidth * archers[i].frame, 0, frameIdleWidth, archerIdeleTexture.height };
            Rectangle destIdel = { archers[i].pos.x, archers[i].pos.y - 32, frameIdleWidth, archerIdeleTexture.height };
            DrawTexturePro(archerIdeleTexture, srcIdel, destIdel,
                (Vector2){frameIdleWidth / 2, archerIdeleTexture.height / 2}, 0.0f, WHITE);
        }
    }
}

void DrawArrows() {
    for (int i = 0; i < MAX_ARROWS; i++) {
        if (!arrows[i].active) continue;

        float dx = arrows[i].target.x - arrows[i].pos.x;
        float dy = arrows[i].target.y - arrows[i].pos.y;
        float angle = atan2f(dy, dx) * RAD2DEG;

        Rectangle src = { 0, 0, (float)arrowTexture.width, (float)arrowTexture.height };
        Rectangle dest = { arrows[i].pos.x, arrows[i].pos.y, 32, 32 };
        DrawTexturePro(arrowTexture, src, dest, (Vector2){16, 16}, angle, WHITE);
    }
}

void RecenterTowers(int newWidth, int newHeight) {
    float scaleX = (float)newWidth / 1280.0f;
    float scaleY = (float)newHeight / 720.0f;

    for (int i = 0; i < towerCount; i++) {
        towers[i].pos.x = towers[i].basePos.x * scaleX;
        towers[i].pos.y = towers[i].basePos.y * scaleY;
        towers[i].size = 64 * scaleX;
    }

    for (int i = 0; i < archerCount; i++) {
        archers[i].pos.x *= scaleX;
        archers[i].pos.y *= scaleY;
    }
}

void UnloadPlayer() {
    UnloadTexture(torreTexture);
    UnloadTexture(archerTexture);
    UnloadTexture(archerIdeleTexture);
    UnloadTexture(arrowTexture);
}