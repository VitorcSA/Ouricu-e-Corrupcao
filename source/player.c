#include <raylib.h>
#include "player.h"
#include "enemies.h"
#include <math.h>
#include <raymath.h>
#include "HUD.h"
#include <stdio.h>

#define ATTACK_RANGE_ARCHER 225.0f
#define ATTACK_RANGE_WIZARD 135.0f
#define GRID_SIZE 64.0f
#define ATTACK_RANGE_CANNON (GRID_SIZE * 5)

Tower towers[MAX_TOWERS];

Players archers[MAX_ARCHERS];
Players wizards[MAX_WIZARDS];
Players cannons[MAX_CANNONS];
Projects arrows[MAX_ARROWS];
Projects fireballs[MAX_FIREBALLS];
Projects cannonballs[MAX_CANNONBALLS];

Texture2D torreTexture;
Texture2D archerTexture;
Texture2D archerIdeleTexture;
Texture2D arrowTexture;
Texture2D wizardTexture;
Texture2D idlewizardTexture;
Texture2D fireballTexture;
Texture2D cannonTextureIdle;
Texture2D cannonTextureShot;
Texture2D cannonballTexture;

int playerGold = 0;
int towerCount = 0;
int archerCount = 0;
int wizardCount = 0;
int cannonCount = 0;

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
}

bool IsTowerOnGrid(Vector2 gridPos) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    float cellWidth = (float)screenWidth / COLS;
    float cellHeight = (float)screenHeight / ROWS;

    // Verifica se já existe torre na mesma célula (x,y)
    for (int i = 0; i < MAX_TOWERS; i++) {
        if (towers[i].active) {
            int towerGridX = (int)(towers[i].pos.x / cellWidth);
            int towerGridY = (int)(towers[i].pos.y / cellHeight);

            if ((int)gridPos.x == towerGridX && (int)gridPos.y == towerGridY) {
                return true;
            }
        }
    }
    return false;
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

void AddPlayer(Players *player, Vector2 pos, int max, int *playerCount, float screenWidth, float screenHeight){
    if (*playerCount >= max) return;
    player[*playerCount].pos = pos;
    player[*playerCount].basePos = (Vector2){ pos.x / (screenWidth/ 1280.0f),
                                              pos.y / (screenHeight / 720.0f) };
    player[*playerCount].active = true;
    player[*playerCount].frame = 0;
    player[*playerCount].frameTime = 0;
    player[*playerCount].shotTimer = 0;
    player[*playerCount].isShooting = false;
    *playerCount += 1;
}

void shootProject(Projects *project, Vector2 start, Vector2 target, float speed, int enemyIndex, int max, bool hasFrames){
    for (int i = 0; i < max; i++) {
        if (!project[i].active) {
            project[i].pos = start;
            project[i].target = target;
            project[i].speed = speed;
            project[i].enemyIndex = enemyIndex;
            project[i].active = true;

            if(hasFrames){
            project[i].frame = 0;
            project[i].frameTime = 0;
            }
            break;
        }
    }
}

void UpdatePlayer(void)
{
    float dt = GetFrameTime();

    // === HUD de seleção de defensor ===
    if (HUD_IsActive()) {
        HUD_Update();
        UnitType selected = HUD_GetSelectedUnit();
        int selTower = HUD_GetSelectedTower();

        if (selTower >= 0 && selTower < towerCount && towers[selTower].active) {
            if (!towers[selTower].hasDefender) { // impede mais de um defensor por torre
                switch (selected) {
                    case UNIT_ARCHER:
                        AddPlayer(archers, (Vector2){ towers[selTower].pos.x, towers[selTower].pos.y - 32 }, MAX_ARCHERS, &archerCount, (float)GetScreenWidth(), (float)GetScreenHeight());
                        towers[selTower].hasDefender = true;
                        break;

                    case UNIT_WIZARD:
                        AddPlayer(wizards, (Vector2){ towers[selTower].pos.x + 20, towers[selTower].pos.y - 45 }, MAX_WIZARDS, &wizardCount, (float)GetScreenWidth(), (float)GetScreenHeight());
                        towers[selTower].hasDefender = true;
                        break;

                    case UNIT_CANNON:
                        AddPlayer(cannons, (Vector2){ towers[selTower].pos.x, towers[selTower].pos.y - 8 }, MAX_CANNONBALLS, &cannonCount, (float)GetScreenWidth(), (float)GetScreenHeight());
                        towers[selTower].hasDefender = true;
                        break;

                    default:
                        break;
                }
            } else {
                printf("Essa torre já tem um defensor!\n");
            }
        }
        return;
    }

    // === Criar torre (só 1 por grid) ===
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();

        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
        float cellWidth = (float)screenWidth / COLS;
        float cellHeight = (float)screenHeight / ROWS;

        Vector2 gridPos = {
            mousePos.x / cellWidth,
            mousePos.y / cellHeight
        };

        int gridX = (int)(mousePos.x / cellWidth);
        int gridY = (int)(mousePos.y / cellHeight);

        Vector2 cellCenter = {
            gridX * cellWidth + (cellWidth * 0.5f) - 8,
            gridY * cellHeight + (cellHeight * 0.5f) - 16
        };

        if (!IsTowerOnGrid(gridPos)) {
            AddTower(cellCenter);
        } else {
            printf("Já existe uma torre nesse grid!\n");
        }
    }

    // === Abrir HUD de torre (botão direito) ===
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Vector2 mouse = GetMousePosition();
        for (int i = 0; i < towerCount; i++) {
            if (towers[i].active && CheckCollisionPointCircle(mouse, towers[i].pos, towers[i].size / 2)) {
                HUD_ShowAt(towers[i].pos, i);
                return;
            }
        }
    }

    // === Arqueiros ===
    for (int i = 0; i < archerCount; i++) {
        if (!archers[i].active) continue;

        // animação
        archers[i].frameTime += dt;
        if (archers[i].frameTime >= 0.1f) {
            archers[i].frame = (archers[i].frame + 1) % ARCHER_QT_FRAMES_SHOOT;
            archers[i].frameTime = 0;
        }

        // ataque
        archers[i].shotTimer -= dt;
        if (archers[i].shotTimer <= 0) {
            bool foundTarget = false;

            // inimigos normais
            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (!enemies[e].active) continue;

                float dist = Vector2Distance(archers[i].pos, enemies[e].pixelPos);

                if (dist <= ATTACK_RANGE_ARCHER) {
                    archers[i].isShooting = true;
                    shootProject(arrows, archers[i].pos, enemies[e].pixelPos, ARROW_SPEED, e, MAX_ARROWS, false);
                    archers[i].shotTimer = ARCHER_SHOT_TIME;
                    foundTarget = true;
                    break;
                }
            }

            // orcs
                for (int e = 0; e < MAX_ORCS; e++) {
                    if (!orcs[e].active) continue;
                    float dist = Vector2Distance(archers[i].pos, orcs[e].pos);
                    if (dist <= ATTACK_RANGE_ARCHER) {
                        archers[i].isShooting = true;
                        shootProject(arrows, archers[i].pos, orcs[e].pos, ARROW_SPEED, e, MAX_ARROWS, false);
                        archers[i].shotTimer = 1.0f;
                        foundTarget = true;
                        break;
                    }
                }

            if (!foundTarget) archers[i].isShooting = false;
        }
    }
    for (int i = 0; i < wizardCount; i++) {
        if (!wizards[i].active) continue;

        wizards[i].frameTime += dt;
        int totalFrames = wizards[i].isShooting ? WIZARD_QT_FRAMES_IDLE : WIZARD_QT_FRAMES_SHOOT;

        if (wizards[i].frameTime >= 0.12f) {
            wizards[i].frame = (wizards[i].frame + 1) % totalFrames;
            wizards[i].frameTime = 0;
        }

        wizards[i].shotTimer -= dt;

        bool foundNear = false;

        for (int e = 0; e < MAX_ENEMIES; e++) {
            if (!enemies[e].active) continue;

            float dist = Vector2Distance(wizards[i].pos, enemies[e].pixelPos);
            if (dist <= GRID_SIZE * 3) foundNear = true;
        }

        wizards[i].isShooting = !foundNear;

        if (wizards[i].shotTimer <= 0) {

            bool attack = false;
            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (!enemies[e].active) continue;

                float dist = Vector2Distance(wizards[i].pos, enemies[e].pixelPos);
                if (dist <= ATTACK_RANGE_WIZARD) {
                    wizards[i].isShooting = true;
                    shootProject(fireballs, wizards[i].pos, enemies[e].pixelPos, FIREBALL_SPEED, e, MAX_FIREBALLS, true);
                    wizards[i].shotTimer = WIZARD_SHOT_TIME;
                    attack = true;
                    break;
                }
            }

            for (int e = 0; e < MAX_ORCS; e++) {
                if (!orcs[e].active) continue;

                float dist = Vector2Distance(wizards[i].pos, orcs[e].pos);
                if (dist <= ATTACK_RANGE_WIZARD) {
                    wizards[i].isShooting = true;
                    shootProject(fireballs, wizards[i].pos, orcs[e].pos, FIREBALL_SPEED, e, MAX_FIREBALLS, true);
                    wizards[i].shotTimer = WIZARD_SHOT_TIME;
                    attack = true;
                    break;
                }
            }

            if (!attack) wizards[i].isShooting = false;
        }
    }

    // === Canhões ===
    for (int i = 0; i < cannonCount; i++) {
        if (!cannons[i].active) continue;
        cannons[i].frameTime += dt;
        if (cannons[i].frameTime >= 0.1f) {
            cannons[i].frame = (cannons[i].frame + 1) % CANNON_QT_FRAMES_SHOOT;
            cannons[i].frameTime = 0;
        }

        cannons[i].shotTimer -= dt;
        if (cannons[i].shotTimer <= 0) {
            bool foundTarget = false;

            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (!enemies[e].active) continue;

                float dist = Vector2Distance(cannons[i].pos, enemies[e].pixelPos);
                if (dist <= ATTACK_RANGE_CANNON) {
                    cannons[i].isShooting = true;

                    shootProject(cannonballs, cannons[i].pos, enemies[e].pixelPos, CANNONBALL_SPEED, e, MAX_CANNONBALLS, false);
                    cannons[i].shotTimer = CANNON_SHOT_TIME;
                    foundTarget = true;
                    break;
                }
            }
            for (int e = 0; e < MAX_ORCS; e++) {
                if (!orcs[e].active) continue;
                float dist = Vector2Distance(cannons[i].pos, orcs[e].pos);
                if (dist <= ATTACK_RANGE_CANNON) {
                    cannons[i].isShooting = true;
                    shootProject(cannonballs, cannons[i].pos, orcs[e].pos, CANNONBALL_SPEED, e, MAX_CANNONBALLS, false);
                    cannons[i].shotTimer = CANNON_SHOT_TIME;
                    foundTarget = true;
                    break;
                }
            }
            if (!foundTarget) cannons[i].isShooting = false;
        }
    }

    // === Flechas ===
    for (int i = 0; i < MAX_ARROWS; i++) {
        if (!arrows[i].active) continue;
        Vector2 dir = Vector2Normalize(Vector2Subtract(arrows[i].target, arrows[i].pos));
        arrows[i].pos = Vector2Add(arrows[i].pos, Vector2Scale(dir, arrows[i].speed * dt));
        if (Vector2Distance(arrows[i].pos, arrows[i].target) < 5.0f) {
            int e = arrows[i].enemyIndex;
            int x = arrows[i].orcIndex;
            if (e >= 0 && e < MAX_ENEMIES && enemies[e].active) {
                enemies[e].health -= ARROW_DAMAGE;
                if (enemies[e].health <= 0) {
                    enemies[e].active = false;
                    playerGold += GOLD_FOR_ENEMY;
                }
            }
            if (x >= 0 && x < MAX_ORCS && orcs[x].active) {
                orcs[x].health -= ARROW_DAMAGE;
                if (orcs[x].health <= 0) {
                    orcs[x].active = false;
                    playerGold += GOLD_FOR_ORC;
                }
            }
            arrows[i].active = false;
        }
    }

    // === Bolas de fogo ===
    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (!fireballs[i].active) continue;
        fireballs[i].frameTime += dt;
        if (fireballs[i].frameTime >= 0.08f) {
            fireballs[i].frame = (fireballs[i].frame + 1) % FIREBALL_QT_FRAMES;
            fireballs[i].frameTime = 0;
        }

        Vector2 dir = Vector2Normalize(Vector2Subtract(fireballs[i].target, fireballs[i].pos));
        fireballs[i].pos = Vector2Add(fireballs[i].pos, Vector2Scale(dir, fireballs[i].speed * dt));
        if (Vector2Distance(fireballs[i].pos, fireballs[i].target) < 12.0f) {
            int e = fireballs[i].enemyIndex;
            int x = fireballs[i].orcIndex;
            if (e >= 0 && e < MAX_ENEMIES && enemies[e].active) {
                enemies[e].health -= FIREBALL_DAMAGE;
                if (enemies[e].health <= 0) {
                    enemies[e].active = false;
                    playerGold += GOLD_FOR_ENEMY;
                }
            }
            if (x >= 0 && x < MAX_ORCS && orcs[x].active) {
                orcs[x].health -= FIREBALL_DAMAGE;
                if (orcs[x].health <= 0) {
                    orcs[x].active = false;
                    playerGold += GOLD_FOR_ORC;
                }
            }
            fireballs[i].active = false;
        }
    }

    // === Balas de canhão ===
    for (int i = 0; i < MAX_CANNONBALLS; i++) {
        if (!cannonballs[i].active) continue;
        Vector2 dir = Vector2Normalize(Vector2Subtract(cannonballs[i].target, cannonballs[i].pos));
        cannonballs[i].pos = Vector2Add(cannonballs[i].pos, Vector2Scale(dir, cannonballs[i].speed * dt));
        if (Vector2Distance(cannonballs[i].pos, cannonballs[i].target) < 5.0f) {
            int e = cannonballs[i].enemyIndex;
            int x = cannonballs[i].orcIndex;
            if (e >= 0 && e < MAX_ENEMIES && enemies[e].active) {
                enemies[e].health -= CANNONBALL_DAMAGE;
                if (enemies[e].health <= 0) {
                    enemies[e].active = false;
                    playerGold += GOLD_FOR_ENEMY;
                }
            }
            if (e >= 0 && e < MAX_ORCS && orcs[x].active) {
                orcs[x].health -= CANNONBALL_DAMAGE;
                if (orcs[x].health <= 0) {
                    orcs[x].active = false;
                    playerGold += GOLD_FOR_ORC;
                }
            }
            cannonballs[i].active = false;
        }
    }
}

// ------------------------------------------------------
// Desenho
// ------------------------------------------------------
void DrawTowers() {
    Vector2 mousePos = GetMousePosition();
    for (int i = 0; i < towerCount; i++) {
        if (towers[i].active) {
            DrawTextureEx(  torreTexture,
                (Vector2){  towers[i].pos.x - towers[i].size / 2,
                            towers[i].pos.y - towers[i].size / 2},
                            0.0f, 
                            1.0f, 
                            WHITE );
        }
    }
}
void DrawPlayer(Players *player, Texture2D playerIdleTexture, Texture2D playerShootingTexture, int playerCount, int quantFrameShot, int quantFrameIdle){
    int frameWidth = playerShootingTexture.width / quantFrameShot;
    int frameIdleWidth = playerIdleTexture.width / quantFrameIdle;

    for (int i = 0; i < playerCount; i++) {
        if (!player[i].active) continue;
        Rectangle src, dest;
        if (player[i].isShooting) {
            src = (Rectangle){ frameWidth * player[i].frame, 0, frameWidth, playerShootingTexture.height };
            dest = (Rectangle){ player[i].pos.x, player[i].pos.y - 32, frameWidth, playerShootingTexture.height };
            DrawTexturePro(playerShootingTexture, src, dest, (Vector2){ frameWidth / 2, playerShootingTexture.height / 2 }, 0.0f, WHITE);
        } else {
            src = (Rectangle){ frameIdleWidth * player[i].frame, 0, frameIdleWidth, playerIdleTexture.height };
            dest = (Rectangle){ player[i].pos.x, player[i].pos.y - 32, frameIdleWidth, playerIdleTexture.height };
            DrawTexturePro(playerIdleTexture, src, dest, (Vector2){ frameIdleWidth / 2, playerIdleTexture.height / 2 }, 0.0f, WHITE);
        }
    }
}

void drawProjects(Projects *project, Texture2D projectTexture, bool hasFrames, int max, int quantFrames){
    for (int i = 0; i < max; i++) {
        if (!project[i].active) continue;

        if(!hasFrames){
            float dx = project[i].target.x - project[i].pos.x;
            float dy = project[i].target.y - project[i].pos.y;

            float angle = atan2f(dy, dx) * RAD2DEG;
            
            Rectangle src = { 0, 0, (float)projectTexture.width, (float)projectTexture.height };
            Rectangle dest = { project[i].pos.x, project[i].pos.y, (float)projectTexture.width, (float)projectTexture.height };

            DrawTexturePro( projectTexture, 
                            src, 
                            dest, 
                            (Vector2){ projectTexture.width/2, projectTexture.height/2 }, 
                            angle, 
                            WHITE );
        }else{
            int frameWidth = projectTexture.width / quantFrames;

            Rectangle src = { frameWidth * project[i].frame, 0, frameWidth, projectTexture.height };
            Rectangle dest = { project[i].pos.x, project[i].pos.y, frameWidth, projectTexture.height };

            DrawTexturePro( projectTexture, 
                            src, 
                            dest, 
                            (Vector2){ frameWidth/2, projectTexture.height/2 }, 
                            0.0f, 
                            WHITE );
        }
    }
}

// ------------------------------------------------------
// Reposicionamento e descarte
// ------------------------------------------------------
void recenterPlayers(Players *player, int playerCount, int scaleX, int scaleY ){
    for (int i = 0; i < playerCount; i++) {
        player[i].pos.x = player[i].basePos.x * scaleX;
        player[i].pos.y = player[i].basePos.y * scaleY;
    }
}

void RecenterTowers(int newWidth, int newHeight)
{
    float scaleX = (float)newWidth / 1280.0f;
    float scaleY = (float)newHeight / 720.0f;

    for (int i = 0; i < towerCount; i++) {
        towers[i].pos.x = towers[i].basePos.x * scaleX;
        towers[i].pos.y = towers[i].basePos.y * scaleY;
        towers[i].size = 64 * scaleX;
    }

    recenterPlayers(archers, archerCount, scaleX, scaleY);
    recenterPlayers(cannons, cannonCount, scaleX, scaleY);
    recenterPlayers(wizards, wizardCount, scaleX, scaleY);
    
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