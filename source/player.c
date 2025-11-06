#include <raylib.h>
#include "player.h"
#include "enemies.h"
#include <math.h>

#define MAX_TOWERS 50
#define MAX_ARCHERS 50
#define ATTACK_RANGE 225.0f

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
} Archer;

static Tower towers[MAX_TOWERS];
static Archer archers[MAX_ARCHERS];
static int towerCount = 0;
static int archerCount = 0;

static Texture2D torreTexture;
static Texture2D archerTexture;

// -----------------------------------------------------------
// Inicialização
// -----------------------------------------------------------
void InitPlayer() {
    Image torre = LoadImage("assets/torre_colocar.png");
    ImageResize(&torre, 64, 64);
    torreTexture = LoadTextureFromImage(torre);
    UnloadImage(torre);

    Image arqueiro = LoadImage("assets/Shot.png");
    archerTexture = LoadTextureFromImage(arqueiro);
    UnloadImage(arqueiro);
}

// -----------------------------------------------------------
// Adiciona uma torre
// -----------------------------------------------------------
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

// -----------------------------------------------------------
// Adiciona um arqueiro (apenas se clicou em uma torre)
// -----------------------------------------------------------
void AddArcher(Vector2 pos) {
    if (archerCount >= MAX_ARCHERS) return;
    archers[archerCount].pos = pos;
    archers[archerCount].active = true;
    archers[archerCount].frame = 0;
    archers[archerCount].frameTime = 0;
    archers[archerCount].shotTimer = 0;
    archerCount++;
}

// -----------------------------------------------------------
// Atualiza jogador (coloca torres ou arqueiros, atira em inimigos)
// -----------------------------------------------------------
void UpdatePlayer() {
    // Colocar torre com botão esquerdo
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
            // procura inimigos próximos
            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (!enemies[e].active) continue;

                float dx = enemies[e].pos.x - archers[i].pos.x;
                float dy = enemies[e].pos.y - archers[i].pos.y;
                float dist = sqrtf(dx * dx + dy * dy);

                if (dist <= ATTACK_RANGE) {
                    enemies[e].health -= 10;
                    if (enemies[e].health <= 0) enemies[e].active = false;

                    // feedback visual (flecha simples)
                    DrawLineV(archers[i].pos, enemies[e].pos, GOLD);
                    archers[i].shotTimer = 1.0f; // atira a cada 1s
                    break;
                }
            }
        }
    }
}

// -----------------------------------------------------------
// Desenha torres
// -----------------------------------------------------------
void DrawTowers() {
    for (int i = 0; i < towerCount; i++) {
        if (towers[i].active)
            DrawTextureEx(torreTexture,
                (Vector2){towers[i].pos.x - towers[i].size / 2, towers[i].pos.y - towers[i].size / 2},
                0.0f, towers[i].size / 64.0f, WHITE);
    }
}

// -----------------------------------------------------------
// Desenha arqueiros animados
// -----------------------------------------------------------
void DrawArchers() {
    int frameWidth = archerTexture.width / 14;
    for (int i = 0; i < archerCount; i++) {
        if (!archers[i].active) continue;

        Rectangle src = { frameWidth * archers[i].frame, 0, frameWidth, archerTexture.height };
        Rectangle dest = { archers[i].pos.x, archers[i].pos.y - 32, frameWidth, archerTexture.height };
        DrawTexturePro(archerTexture, src, dest,
            (Vector2){frameWidth / 2, archerTexture.height / 2}, 0.0f, WHITE);
    }
}

// -----------------------------------------------------------
// Reposiciona torres (para fullscreen etc.)
// -----------------------------------------------------------
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

// -----------------------------------------------------------
// Descarrega texturas
// -----------------------------------------------------------
void UnloadPlayer() {
    UnloadTexture(torreTexture);
    UnloadTexture(archerTexture);
}