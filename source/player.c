#include <raylib.h>
#include "player.h"
#include "enemies.h"
#include <math.h>

#define MAX_TOWERS 50
#define MAX_ARCHERS 50
#define ATTACK_RANGE 225.0f
#define GRID_SIZE 64.0f

typedef struct {
    Vector2 pos;
    Vector2 basePos;
    float size;
    bool active;
} Tower;

typedef struct {
    Vector2 pos;
    bool active;
    bool attacking;
    int frame;
    float frameTime;
    float shotTimer;
} Archer;

static Tower towers[MAX_TOWERS];
static Archer archers[MAX_ARCHERS];
static int towerCount = 0;
static int archerCount = 0;
static int idleFrameCount = 9;
static int shotFrameCount = 14;
static float idleFrameSpeed = 0.12f;
static float shotFrameSpeed = 0.08f;


static Texture2D torreTexture;
static Texture2D archerIdleTexture;
static Texture2D archerShotTexture;

void InitPlayer() {
    // Torre
    Image torre = LoadImage("assets/torre_colocar.png");
    ImageResize(&torre, 64, 64);
    torreTexture = LoadTextureFromImage(torre);
    UnloadImage(torre);

    // Arqueiro Idle
    Image idle = LoadImage("assets/inimigosAnimation/Idle.png");
    archerIdleTexture = LoadTextureFromImage(idle);
    UnloadImage(idle);

    // Arqueiro Atirando
    Image shot = LoadImage("assets/inimigosAnimation/Shot.png");
    archerShotTexture = LoadTextureFromImage(shot);
    UnloadImage(shot);

    // Força número de frames correto (Idle tem 9, Shot tem 14)
    idleFrameCount = 9;
    shotFrameCount = 14;
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
    archers[archerCount].attacking = false;
    archers[archerCount].frame = 0;
    archers[archerCount].frameTime = 0;
    archers[archerCount].shotTimer = 0;
    archerCount++;
}

void UpdatePlayer() {
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

        // Verifica inimigo mais próximo
        float nearest = 99999.0f;
        for (int e = 0; e < MAX_ENEMIES; e++) {
            if (!enemies[e].active) continue;
            float dx = enemies[e].pos.x - archers[i].pos.x;
            float dy = enemies[e].pos.y - archers[i].pos.y;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist < nearest) nearest = dist;
        }

        // Define se está atacando
        bool inRange = nearest <= (GRID_SIZE * 4);
        archers[i].attacking = inRange;

        // Atualiza animação (Idle e Shot animam igual)
        archers[i].frameTime += dt;

        int maxFrames = archers[i].attacking ? shotFrameCount : idleFrameCount;
        float frameSpeed = archers[i].attacking ? shotFrameSpeed : idleFrameSpeed;

        if (archers[i].frame >= maxFrames) archers[i].frame = 0;    

        if (archers[i].frameTime >= frameSpeed) {
        archers[i].frame = (archers[i].frame + 1) % maxFrames;
        archers[i].frameTime = 0;
}

        // Se estiver atacando, causar dano periódico
        if (archers[i].attacking) {
            archers[i].shotTimer -= dt;
            if (archers[i].shotTimer <= 0) {
                for (int e = 0; e < MAX_ENEMIES; e++) {
                    if (!enemies[e].active) continue;

                    float dx = enemies[e].pos.x - archers[i].pos.x;
                    float dy = enemies[e].pos.y - archers[i].pos.y;
                    float dist = sqrtf(dx * dx + dy * dy);
                    if (dist <= ATTACK_RANGE) {
                        enemies[e].health -= 10;
                        if (enemies[e].health <= 0) enemies[e].active = false;
                        DrawLineV(archers[i].pos, enemies[e].pos, GOLD);
                        break;
                    }
                }
                archers[i].shotTimer = 1.0f;
            }
        }
    }
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
    for (int i = 0; i < archerCount; i++) {
        if (!archers[i].active) continue;

        Texture2D tex = archers[i].attacking ? archerShotTexture : archerIdleTexture;
        int frameCount = archers[i].attacking ? 14 : 9;  // Idle agora anima igual ao Shot
        int frameWidth = tex.width / frameCount;

        Rectangle src = { frameWidth * archers[i].frame, 0, frameWidth, tex.height };
        Rectangle dest = { archers[i].pos.x, archers[i].pos.y - 32, frameWidth, tex.height };
        DrawTexturePro(tex, src, dest,
            (Vector2){frameWidth / 2, tex.height / 2}, 0.0f, WHITE);
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
    UnloadTexture(archerIdleTexture);
    UnloadTexture(archerShotTexture);
}