#include <raylib.h>
#include "player.h"
#include "enemies.h"
#include <math.h>
#include <raymath.h>
#include "HUD.h"

#define MAX_TOWERS 50
#define MAX_ARCHERS 50
#define MAX_WIZARDS 50
#define MAX_CANNONS 50
#define MAX_ARROWS 200
#define MAX_FIREBALLS 200
#define MAX_CANNONBALLS 200

#define ATTACK_RANGE_ARCHER 225.0f
#define ATTACK_RANGE_WIZARD 135.0f
#define GRID_SIZE 64.0f
#define ATTACK_RANGE_CANNON (GRID_SIZE * 5)

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
    float size;
    bool isAttacking;
    bool isIdle;
} Shotwizard;

typedef struct {
    Vector2 pos;
    bool active;
    float size;
    bool isShooting;
    int frame;
    float frameTime;
    float shotTimer;
} Cannon;

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

typedef struct {
    Vector2 pos;
    Vector2 target;
    bool active;
    float speed;
    int enemyIndex;
} Cannonball;

static Tower towers[MAX_TOWERS];
static Archer archers[MAX_ARCHERS];
static Shotwizard wizards[MAX_WIZARDS];
static Cannon cannons[MAX_CANNONS];
static Arrow arrows[MAX_ARROWS];
static Fireball fireballs[MAX_FIREBALLS];
static Cannonball cannonballs[MAX_CANNONBALLS];

static int towerCount = 0;
static int archerCount = 0;
static int wizardCount = 0;
static int cannonCount = 0;

static Texture2D torreTexture;
static Texture2D archerTexture;
static Texture2D archerIdeleTexture;
static Texture2D arrowTexture;
static Texture2D wizardTexture;
static Texture2D idlewizardTexture;
static Texture2D fireballTexture;
static Texture2D cannonTextureIdle;
static Texture2D cannonTextureShot;
static Texture2D cannonballTexture;

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

    Image cannonIdle = LoadImage("assets/inimigosAnimation/idlecannon.png");
    cannonTextureIdle = LoadTextureFromImage(cannonIdle);
    UnloadImage(cannonIdle);

    Image cannonShot = LoadImage("assets/inimigosAnimation/shotcannon.png");
    cannonTextureShot = LoadTextureFromImage(cannonShot);
    UnloadImage(cannonShot);

    Image cannonball = LoadImage("assets/inimigosAnimation/cannonball.png");
    cannonballTexture = LoadTextureFromImage(cannonball);
    UnloadImage(cannonball);

    for (int i = 0; i < MAX_FIREBALLS; i++) {
        fireballs[i].active = false;
        fireballs[i].frame = 0;
        fireballs[i].frameTime = 0;
        fireballs[i].speed = 400.0f;
    }
}

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

void AddWizard(Vector2 towerPos, float towerSize)
{
    if (wizardCount >= MAX_WIZARDS) return;
    wizards[wizardCount].pos.x = towerPos.x + 35;
    wizards[wizardCount].pos.y = towerPos.y - (towerSize / 2) + (wizards[wizardCount].size / 2);
    wizards[wizardCount].active = true;
    wizards[wizardCount].frame = 0;
    wizards[wizardCount].frameTime = 0;
    wizards[wizardCount].shotTimer = 0;
    wizards[wizardCount].isAttacking = false;
    wizards[wizardCount].isIdle = true;
    wizardCount++;
}

void AddCannon(Vector2 towerPos, float towerSize)
{
    if (cannonCount >= MAX_CANNONS) return;

    cannons[cannonCount].size = 64.0f;
    cannons[cannonCount].pos.x = towerPos.x + 35;
    cannons[cannonCount].pos.y = towerPos.y - (towerSize / 2) + (cannons[cannonCount].size / 2);
    cannons[cannonCount].active = true;
    cannons[cannonCount].frame = 0;
    cannons[cannonCount].frameTime = 0;
    cannons[cannonCount].shotTimer = 0;
    cannons[cannonCount].isShooting = false;
    cannonCount++;
}

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

void ShootCannonball(Vector2 start, Vector2 target, int enemyIndex)
{
    for (int i = 0; i < MAX_CANNONBALLS; i++) {
        if (!cannonballs[i].active) {
            cannonballs[i].pos = start;
            cannonballs[i].target = target;
            cannonballs[i].speed = 600.0f;
            cannonballs[i].enemyIndex = enemyIndex;
            cannonballs[i].active = true;
            break;
        }
    }
}

void UpdatePlayer(void)
{
    float dt = GetFrameTime();

    // HUD
    if (HUD_IsActive()) {
        HUD_Update();
        UnitType selected = HUD_GetSelectedUnit();
        int selTower = HUD_GetSelectedTower();

        if (selTower >= 0 && selTower < towerCount) {
            if (selected == UNIT_ARCHER) {
                AddArcher((Vector2){towers[selTower].pos.x, towers[selTower].pos.y - 32});
            } else if (selected == UNIT_WIZARD) {
                AddWizard(towers[selTower].pos, towers[selTower].size);
            } else if (selected == UNIT_CANNON && selTower >= 0 && selTower < towerCount) {
                AddCannon(towers[selTower].pos, towers[selTower].size);
            }
        }
        return;
    }

    // Criar torre
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();
        AddTower(mouse);
    }

    // Abrir HUD
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Vector2 mouse = GetMousePosition();
        for (int i = 0; i < towerCount; i++) {
            if (towers[i].active && CheckCollisionPointCircle(mouse, towers[i].pos, towers[i].size / 2)) {
                HUD_ShowAt(towers[i].pos, i);
                return;
            }
        }
    }

    // --- Atualizar arqueiros ---
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

    // --- Atualizar magos ---
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
            if (dist <= GRID_SIZE * 3) foundNear = true;
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

    for (int i = 0; i < cannonCount; i++) {
        if (!cannons[i].active) continue;
        cannons[i].frameTime += dt;
        if (cannons[i].frameTime >= 0.1f) {
            cannons[i].frame = (archers[i].frame + 1) % 5;
            cannons[i].frameTime = 0;
        }

        cannons[i].shotTimer -= dt;
        if (cannons[i].shotTimer <= 0) {
            bool foundTarget = false;
            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (!enemies[e].active) continue;
                float dist = Vector2Distance(cannons[i].pos, enemies[e].pos);
                if (dist <= ATTACK_RANGE_CANNON) {
                    cannons[i].isShooting = true;
                    ShootCannonball(cannons[i].pos, enemies[e].pos, e);
                    cannons[i].shotTimer = 1.0f;
                    foundTarget = true;
                    break;
                }
            }
            if (!foundTarget) cannons[i].isShooting = false;
        }
    }

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

    // (Fireballs)
    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (!fireballs[i].active) continue;
        fireballs[i].frameTime += dt;
        if (fireballs[i].frameTime >= 0.08f) {
            fireballs[i].frame = (fireballs[i].frame + 1) % 12;
            fireballs[i].frameTime = 0;
        }

        Vector2 dir = Vector2Normalize(Vector2Subtract(fireballs[i].target, fireballs[i].pos));
        fireballs[i].pos = Vector2Add(fireballs[i].pos, Vector2Scale(dir, fireballs[i].speed * dt));
        if (Vector2Distance(fireballs[i].pos, fireballs[i].target) < 12.0f) {
            int e = fireballs[i].enemyIndex;
            if (e >= 0 && e < MAX_ENEMIES && enemies[e].active) {
                enemies[e].health -= 25;
                if (enemies[e].health <= 0) enemies[e].active = false;
            }
            fireballs[i].active = false;
        }
    }

    for (int i = 0; i < MAX_CANNONBALLS; i++) {
        if (!cannonballs[i].active) continue;
        Vector2 dir = Vector2Normalize(Vector2Subtract(cannonballs[i].target, cannonballs[i].pos));
        cannonballs[i].pos = Vector2Add(cannonballs[i].pos, Vector2Scale(dir, cannonballs[i].speed * dt));
        if (Vector2Distance(cannonballs[i].pos, cannonballs[i].target) < 5.0f) {
            int e = cannonballs[i].enemyIndex;
            if (e >= 0 && e < MAX_ENEMIES && enemies[e].active) {
                enemies[e].health -= 40;
                if (enemies[e].health <= 0) enemies[e].active = false;
            }
            cannonballs[i].active = false;
        }
    }
}

// ------------------------------------------------------
// Desenho
// ------------------------------------------------------
void DrawTowers()
{
    for (int i = 0; i < towerCount; i++) {
        if (towers[i].active) {
            DrawTextureEx(torreTexture,
                (Vector2){towers[i].pos.x - towers[i].size / 2, towers[i].pos.y - towers[i].size / 2},
                0.0f, towers[i].size / 64.0f, WHITE);
        }
    }
}

void DrawArchers()
{
    int frameWidth = archerTexture.width / 14;
    int frameIdleWidth = archerIdeleTexture.width / 9;

    for (int i = 0; i < archerCount; i++) {
        if (!archers[i].active) continue;
        Rectangle src, dest;
        if (archers[i].isShooting) {
            src = (Rectangle){ frameWidth * archers[i].frame, 0, frameWidth, archerTexture.height };
            dest = (Rectangle){ archers[i].pos.x, archers[i].pos.y - 32, frameWidth, archerTexture.height };
            DrawTexturePro(archerTexture, src, dest, (Vector2){ frameWidth / 2, archerTexture.height / 2 }, 0.0f, WHITE);
        } else {
            src = (Rectangle){ frameIdleWidth * archers[i].frame, 0, frameIdleWidth, archerIdeleTexture.height };
            dest = (Rectangle){ archers[i].pos.x, archers[i].pos.y - 32, frameIdleWidth, archerIdeleTexture.height };
            DrawTexturePro(archerIdeleTexture, src, dest, (Vector2){ frameIdleWidth / 2, archerIdeleTexture.height / 2 }, 0.0f, WHITE);
        }
    }
}

void DrawWizards()
{
    int frameWidthShot = wizardTexture.width / 8;
    int frameWidthIdle = idlewizardTexture.width / 7;

    for (int i = 0; i < wizardCount; i++) {
        if (!wizards[i].active) continue;
        Rectangle src, dest;
        if (wizards[i].isIdle) {
            src = (Rectangle){ frameWidthIdle * wizards[i].frame, 0, frameWidthIdle, idlewizardTexture.height };
            dest = (Rectangle){ wizards[i].pos.x, wizards[i].pos.y - 32, frameWidthIdle, idlewizardTexture.height };
            DrawTexturePro(idlewizardTexture, src, dest, (Vector2){ frameWidthIdle / 2, idlewizardTexture.height / 2 }, 0.0f, WHITE);
        } else {
            src = (Rectangle){ frameWidthShot * wizards[i].frame, 0, frameWidthShot, wizardTexture.height };
            dest = (Rectangle){ wizards[i].pos.x, wizards[i].pos.y - 32, frameWidthShot, wizardTexture.height };
            DrawTexturePro(wizardTexture, src, dest, (Vector2){ frameWidthShot / 2, wizardTexture.height / 2 }, 0.0f, WHITE);
        }
    }
}

void DrawCannons()
{
    int frameWidth = cannonTextureShot.width / 5;
    int frameIdleWidth = cannonTextureIdle.width / 1;

    for (int i = 0; i < cannonCount; i++) {
        if (!cannons[i].active) continue;
        Rectangle src, dest;
        if (cannons[i].isShooting) {
            src = (Rectangle){ frameWidth * cannons[i].frame, 0, frameWidth, cannonTextureShot.height };
            dest = (Rectangle){ cannons[i].pos.x - 30, cannons[i].pos.y - 32, frameWidth, cannonTextureShot.height };
            DrawTexturePro(cannonTextureShot, src, dest, (Vector2){ frameWidth / 2, cannonTextureShot.height / 2 }, 0.0f, WHITE);
        } else {
            src = (Rectangle){ frameIdleWidth * cannons[i].frame, 0, frameIdleWidth, cannonTextureIdle.height };
            dest = (Rectangle){ cannons[i].pos.x - 30, cannons[i].pos.y - 32, frameIdleWidth, cannonTextureIdle.height };
            DrawTexturePro(cannonTextureIdle, src, dest, (Vector2){ frameIdleWidth / 2, cannonTextureIdle.height / 2 }, 0.0f, WHITE);
        }
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
        Rectangle dest = { arrows[i].pos.x, arrows[i].pos.y, (float)arrowTexture.width, (float)arrowTexture.height };
        DrawTexturePro(arrowTexture, src, dest, (Vector2){ arrowTexture.width/2, arrowTexture.height/2 }, angle, WHITE);
    }
}

void DrawFireballs()
{
    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (!fireballs[i].active) continue;
        int frameWidth = fireballTexture.width / 12;
        Rectangle src = { frameWidth * fireballs[i].frame, 0, frameWidth, fireballTexture.height };
        DrawTexturePro(fireballTexture, src, (Rectangle){ fireballs[i].pos.x, fireballs[i].pos.y, frameWidth, fireballTexture.height }, 
            (Vector2){ frameWidth/2, fireballTexture.height/2 }, 0.0f, WHITE);
    }
}

void DrawCannonballs()
{
    for (int i = 0; i < MAX_CANNONBALLS; i++) {
        if (!cannonballs[i].active) continue;
        float dx = cannonballs[i].target.x - cannonballs[i].pos.x;
        float dy = cannonballs[i].target.y - cannonballs[i].pos.y;
        float angle = atan2f(dy, dx) * RAD2DEG;
        Rectangle src = { 0, 0, (float)cannonballTexture.width, (float)cannonballTexture.height };
        Rectangle dest = { cannonballs[i].pos.x, cannonballs[i].pos.y, (float)cannonballTexture.width, (float)cannonballTexture.height };
        DrawTexturePro(cannonballTexture, src, dest, (Vector2){ cannonballTexture.width/2, cannonballTexture.height/2 }, angle, WHITE);
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
    for (int i = 0; i < cannonCount; i++) {
        cannons[i].pos.x *= scaleX;
        cannons[i].pos.y *= scaleY;
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
    UnloadTexture(cannonTextureIdle);
    UnloadTexture(cannonTextureShot);
    UnloadTexture(cannonballTexture);
}