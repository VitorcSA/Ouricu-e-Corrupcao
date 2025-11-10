#include <raylib.h>
#include "player.h"
#include "enemies.h"
#include <math.h>
#include <raymath.h>
#include "HUD.h"

#define MAX_TOWERS 50
#define MAX_ARCHERS 50
#define MAX_WIZARDS 50
#define MAX_ARROWS 200
#define MAX_FIREBALLS 200

#define ATTACK_RANGE_ARCHER 225.0f
#define ATTACK_RANGE_WIZARD 135.0f
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
    int frame;
    float frameTime;
    float shotTimer;
    bool isShooting;
} Archer;

typedef struct {
    Vector2 pos;
    bool active;
    int frame;
    float frameTime;
    float shotTimer;
    bool isAttacking;
    bool isIdle;
} Shotwizard;

typedef struct {
    Vector2 pos;
    Vector2 target;
    bool active;
    float speed;
    int enemyIndex;
} Arrow;

typedef struct {
    Vector2 pos;
    Vector2 target;
    bool active;
    float speed;
    int frame;
    float frameTime;
    int enemyIndex;
} Fireball;

static Arrow arrows[MAX_ARROWS];
static Fireball fireballs[MAX_FIREBALLS];
static Tower towers[MAX_TOWERS];
static Archer archers[MAX_ARCHERS];
static Shotwizard wizards[MAX_WIZARDS];
static int towerCount = 0;
static int archerCount = 0;
static int wizardCount = 0;

static Texture2D torreTexture;
static Texture2D archerTexture;
static Texture2D archerIdeleTexture;
static Texture2D arrowTexture;
static Texture2D wizardTexture;
static Texture2D idlewizardTexture;
static Texture2D fireballTexture;

// ------------------------------------------------------
// Inicialização
// ------------------------------------------------------
void InitPlayer()
{
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

    Image wizard = LoadImage("assets/inimigosAnimation/ShotWizard.png");
    wizardTexture = LoadTextureFromImage(wizard);
    UnloadImage(wizard);

    Image idlewizard = LoadImage("assets/inimigosAnimation/IdleWizard.png");
    idlewizardTexture = LoadTextureFromImage(idlewizard);
    UnloadImage(idlewizard);

    Image fireball = LoadImage("assets/inimigosAnimation/Fireball.png");
    fireballTexture = LoadTextureFromImage(fireball);
    UnloadImage(fireball);

    for (int i = 0; i < MAX_FIREBALLS; i++) {
        fireballs[i].active = false;
        fireballs[i].frame = 0;
        fireballs[i].frameTime = 0;
        fireballs[i].speed = 400.0f;
    }
}

// ------------------------------------------------------
// Torres e Unidades
// ------------------------------------------------------
void AddTower(Vector2 pos)
{
    if (towerCount >= MAX_TOWERS) return;
    towers[towerCount].pos = pos;
    towers[towerCount].basePos = (Vector2){ pos.x / ((float)GetScreenWidth() / 1280.0f),
                                            pos.y / ((float)GetScreenHeight() / 720.0f) };
    towers[towerCount].size = 64;
    towers[towerCount].active = true;
    towerCount++;
}

void AddArcher(Vector2 pos)
{
    if (archerCount >= MAX_ARCHERS) return;
    archers[archerCount].pos = pos;
    archers[archerCount].active = true;
    archers[archerCount].frame = 0;
    archers[archerCount].frameTime = 0;
    archers[archerCount].shotTimer = 0;
    archers[archerCount].isShooting = false;
    archerCount++;
}

void AddWizard(Vector2 pos)
{
    if (wizardCount >= MAX_WIZARDS) return;
    wizards[wizardCount].pos = pos;
    wizards[wizardCount].active = true;
    wizards[wizardCount].frame = 0;
    wizards[wizardCount].frameTime = 0;
    wizards[wizardCount].shotTimer = 0;
    wizards[wizardCount].isAttacking = false;
    wizards[wizardCount].isIdle = true;
    wizardCount++;
}

// ------------------------------------------------------
// Tiros
// ------------------------------------------------------
void ShootArrow(Vector2 start, Vector2 target, int enemyIndex)
{
    for (int i = 0; i < MAX_ARROWS; i++) {
        if (!arrows[i].active) {
            arrows[i].pos = start;
            arrows[i].target = target;
            arrows[i].speed = 600.0f;
            arrows[i].enemyIndex = enemyIndex;
            arrows[i].active = true;
            break;
        }
    }
}

void ShootFireball(Vector2 start, Vector2 target, int enemyIndex)
{
    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (!fireballs[i].active) {
            fireballs[i].pos = start;
            fireballs[i].target = target;
            fireballs[i].speed = 400.0f;
            fireballs[i].enemyIndex = enemyIndex;
            fireballs[i].active = true;
            fireballs[i].frame = 0;
            fireballs[i].frameTime = 0;
            break;
        }
    }
}

void UpdateArrows(float dt)
{
    for (int i = 0; i < MAX_ARROWS; i++) {
        if (!arrows[i].active) continue;
        Vector2 dir = Vector2Normalize(Vector2Subtract(arrows[i].target, arrows[i].pos));
        arrows[i].pos = Vector2Add(arrows[i].pos, Vector2Scale(dir, arrows[i].speed * dt));
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

void UpdateFireballs(float dt)
{
    int frameWidth = fireballTexture.width / 12;
    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (!fireballs[i].active) continue;

        // Animação
        fireballs[i].frameTime += dt;
        if (fireballs[i].frameTime >= 0.08f) {
            fireballs[i].frame = (fireballs[i].frame + 1) % 12;
            fireballs[i].frameTime = 0;
        }

        // Movimento
        Vector2 dir = Vector2Normalize(Vector2Subtract(fireballs[i].target, fireballs[i].pos));
        fireballs[i].pos = Vector2Add(fireballs[i].pos, Vector2Scale(dir, fireballs[i].speed * dt));

        // Colisão
        if (Vector2Distance(fireballs[i].pos, fireballs[i].target) < 12.0f) {
            int e = fireballs[i].enemyIndex;
            if (e >= 0 && e < MAX_ENEMIES && enemies[e].active) {
                enemies[e].health -= 25;
                if (enemies[e].health <= 0) enemies[e].active = false;
            }
            fireballs[i].active = false;
        }
    }
}

// ------------------------------------------------------
// Atualização principal do player
// ------------------------------------------------------
void UpdatePlayer(void)
{
    float dt = GetFrameTime();

    if (HUD_IsActive()) {
        HUD_Update();
        UnitType selected = HUD_GetSelectedUnit();
        int selTower = HUD_GetSelectedTower();
        if (selected == UNIT_ARCHER && selTower >= 0 && selTower < towerCount) {
            AddArcher((Vector2){towers[selTower].pos.x, towers[selTower].pos.y - 32});
        } else if (selected == UNIT_WIZARD && selTower >= 0 && selTower < towerCount) {
            AddWizard((Vector2){towers[selTower].pos.x, towers[selTower].pos.y - 32});
        }
        return;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();
        AddTower(mouse);
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Vector2 mouse = GetMousePosition();
        for (int i = 0; i < towerCount; i++) {
            if (towers[i].active && CheckCollisionPointCircle(mouse, towers[i].pos, towers[i].size / 2)) {
                HUD_ShowAt(towers[i].pos, i);
                return;
            }
        }
    }

    // --- Arqueiros ---
    for (int i = 0; i < archerCount; i++) {
        if (!archers[i].active) continue;
        archers[i].frameTime += dt;
        if (archers[i].frameTime >= 0.1f) {
            archers[i].frame = (archers[i].frame + 1) % 14;
            archers[i].frameTime = 0;
        }

        archers[i].shotTimer -= dt;
        if (archers[i].shotTimer <= 0) {
            bool foundTarget = false;
            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (!enemies[e].active) continue;
                float dist = Vector2Distance(archers[i].pos, enemies[e].pos);
                if (dist <= ATTACK_RANGE_ARCHER) {
                    archers[i].isShooting = true;
                    ShootArrow(archers[i].pos, enemies[e].pos, e);
                    archers[i].shotTimer = 1.0f;
                    foundTarget = true;
                    break;
                }
            }
            if (!foundTarget) archers[i].isShooting = false;
        }
    }

    // --- Magos ---
    for (int i = 0; i < wizardCount; i++) {
        if (!wizards[i].active) continue;
        wizards[i].frameTime += dt;
        int totalFrames = wizards[i].isIdle ? 7 : 8;

        if (wizards[i].frameTime >= 0.12f) {
            wizards[i].frame = (wizards[i].frame + 1) % totalFrames;
            wizards[i].frameTime = 0;
        }

        wizards[i].shotTimer -= dt;
        bool foundNear = false;
        for (int e = 0; e < MAX_ENEMIES; e++) {
            if (!enemies[e].active) continue;
            float dist = Vector2Distance(wizards[i].pos, enemies[e].pos);
            if (dist <= GRID_SIZE * 3) {
                foundNear = true;
                break;
            }
        }

        wizards[i].isIdle = !foundNear;

        if (wizards[i].shotTimer <= 0) {
            bool attack = false;
            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (!enemies[e].active) continue;
                float dist = Vector2Distance(wizards[i].pos, enemies[e].pos);
                if (dist <= ATTACK_RANGE_WIZARD) {
                    wizards[i].isAttacking = true;
                    ShootFireball(wizards[i].pos, enemies[e].pos, e);
                    wizards[i].shotTimer = 1.2f;
                    attack = true;
                    break;
                }
            }
            if (!attack) wizards[i].isAttacking = false;
        }
    }

    UpdateArrows(dt);
    UpdateFireballs(dt);
}

// ------------------------------------------------------
// Desenho
// ------------------------------------------------------
void DrawTowers()
{
    for (int i = 0; i < towerCount; i++) {
        if (towers[i].active)
            DrawTextureEx(torreTexture,
                (Vector2){towers[i].pos.x - towers[i].size / 2, towers[i].pos.y - towers[i].size / 2},
                0.0f, towers[i].size / 64.0f, WHITE);
        HUD_Draw();
    }
}

void DrawArchers()
{
    int frameWidth = archerTexture.width / 14;
    int frameIdleWidth = archerIdeleTexture.width / 9;

    for (int i = 0; i < archerCount; i++) {
        if (!archers[i].active) continue;

        if (archers[i].isShooting) {
            Rectangle src = { frameWidth * archers[i].frame, 0, frameWidth, archerTexture.height };
            Rectangle dest = { archers[i].pos.x, archers[i].pos.y - 32, frameWidth, archerTexture.height };
            DrawTexturePro(archerTexture, src, dest, (Vector2){ frameWidth / 2, archerTexture.height / 2 }, 0.0f, WHITE);
        } else {
            Rectangle srcIdel = { frameIdleWidth * archers[i].frame, 0, frameIdleWidth, archerIdeleTexture.height };
            Rectangle destIdel = { archers[i].pos.x, archers[i].pos.y - 32, frameIdleWidth, archerIdeleTexture.height };
            DrawTexturePro(archerIdeleTexture, srcIdel, destIdel, (Vector2){ frameIdleWidth / 2, archerIdeleTexture.height / 2 }, 0.0f, WHITE);
        }
    }
}

void DrawWizards()
{
    int frameWidthShot = wizardTexture.width / 8;
    int frameWidthIdle = idlewizardTexture.width / 7;

    for (int i = 0; i < wizardCount; i++) {
        if (!wizards[i].active) continue;

        if (wizards[i].isIdle) {
            Rectangle src = { frameWidthIdle * wizards[i].frame, 0, frameWidthIdle, idlewizardTexture.height };
            Rectangle dest = { wizards[i].pos.x, wizards[i].pos.y - 32, frameWidthIdle, idlewizardTexture.height };
            DrawTexturePro(idlewizardTexture, src, dest, (Vector2){ frameWidthIdle / 2, idlewizardTexture.height / 2 }, 0.0f, WHITE);
        } else {
            Rectangle src = { frameWidthShot * wizards[i].frame, 0, frameWidthShot, wizardTexture.height };
            Rectangle dest = { wizards[i].pos.x, wizards[i].pos.y - 32, frameWidthShot, wizardTexture.height };
            DrawTexturePro(wizardTexture, src, dest, (Vector2){ frameWidthShot / 2, wizardTexture.height / 2 }, 0.0f, WHITE);
        }
    }
}

void DrawFireballs()
{
    int frameWidth = fireballTexture.width / 12;

    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (!fireballs[i].active) continue;
        float dx = fireballs[i].target.x - fireballs[i].pos.x;
        float dy = fireballs[i].target.y - fireballs[i].pos.y;
        float angle = atan2f(dy, dx) * RAD2DEG;

        Rectangle src = { frameWidth * fireballs[i].frame, 0, frameWidth, fireballTexture.height };
        Rectangle dest = { fireballs[i].pos.x, fireballs[i].pos.y, 48, 48 };
        DrawTexturePro(fireballTexture, src, dest, (Vector2){24, 24}, angle, WHITE);
    }
}

void DrawArrows()
{
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

// ------------------------------------------------------
// Reposicionamento e descarte
// ------------------------------------------------------
void RecenterTowers(int newWidth, int newHeight)
{
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
    for (int i = 0; i < wizardCount; i++) {
        wizards[i].pos.x *= scaleX;
        wizards[i].pos.y *= scaleY;
    }
}

void UnloadPlayer()
{
    UnloadTexture(torreTexture);
    UnloadTexture(archerTexture);
    UnloadTexture(archerIdeleTexture);
    UnloadTexture(arrowTexture);
    UnloadTexture(wizardTexture);
    UnloadTexture(idlewizardTexture);
    UnloadTexture(fireballTexture);
}