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

int playerGold;
int towerCount = 0;
int archerCount = 0;
int wizardCount = 0;
int cannonCount = 0;
int ownedArchers = 1;
int ownedWizards = 0;
int ownedCannons = 0;
int ownedTowers = 1;

bool wizardUnlocked = false;
bool cannonUnlocked = false;

void InitPlayer()
{
    Image torre = LoadImage("assets/torre_colocar.png");
    ImageResize(&torre, 64, 64);
    torreTexture = LoadTextureFromImage(torre);
    UnloadImage(torre);

    Image arqueiro = LoadImage("assets/InimigosAnimation/Shot.png");
    archerTexture = LoadTextureFromImage(arqueiro);
    UnloadImage(arqueiro);

    Image arqueiroIdele = LoadImage("assets/InimigosAnimation/Idle.png");
    archerIdeleTexture = LoadTextureFromImage(arqueiroIdele);
    UnloadImage(arqueiroIdele);

    Image arrow = LoadImage("assets/InimigosAnimation/Arrow.png");
    arrowTexture = LoadTextureFromImage(arrow);
    UnloadImage(arrow);

    Image wizard = LoadImage("assets/InimigosAnimation/ShotWizard.png");
    wizardTexture = LoadTextureFromImage(wizard);
    UnloadImage(wizard);

    Image idlewizard = LoadImage("assets/InimigosAnimation/IdleWizard.png");
    idlewizardTexture = LoadTextureFromImage(idlewizard);
    UnloadImage(idlewizard);

    Image fireball = LoadImage("assets/InimigosAnimation/Fireball.png");
    fireballTexture = LoadTextureFromImage(fireball);
    UnloadImage(fireball);

    Image cannonIdle = LoadImage("assets/InimigosAnimation/idlecannon.png");
    cannonTextureIdle = LoadTextureFromImage(cannonIdle);
    UnloadImage(cannonIdle);

    Image cannonShot = LoadImage("assets/InimigosAnimation/shotcannon.png");
    cannonTextureShot = LoadTextureFromImage(cannonShot);
    UnloadImage(cannonShot);

    Image cannonball = LoadImage("assets/InimigosAnimation/cannonball.png");
    cannonballTexture = LoadTextureFromImage(cannonball);
    UnloadImage(cannonball);
}

bool IsTowerOnGrid(Vector2 gridPos) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    float cellWidth = (float)screenWidth / COLS;
    float cellHeight = (float)screenHeight / ROWS;

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

bool IsTowerOnPath(Vector2 gridPos, unsigned char *mapa){
    int gridX = (int)(gridPos.x);
    int gridY = (int)(gridPos.y);

    if(mapa[gridY * COLS  + gridX] != 0){
        return true;
    }
    return false;
}

void AddTower(Vector2 pos, int screenWidth, int screenHeight)
{
    if (ownedTowers <= 0 || playerGold < 10) return;

    float cellWidth  = (float)screenWidth / (float)COLS;
    float cellHeight = (float)screenHeight / (float)ROWS;

    towers[towerCount].pos = pos;
    towers[towerCount].basePos = (Vector2){ pos.x / (float)GetScreenWidth(),
                                            pos.y / (float)GetScreenHeight() };
    float s = (cellWidth < cellHeight) ? cellWidth : cellHeight;
    towers[towerCount].size = s * 1.1f;
    towers[towerCount].active = true;

    towerCount++;
    playerGold -= 10;
    if (playerGold < 0)  playerGold = 0;
}

void AddPlayer(Players *player, Vector2 pos, int max, int *playerCount, int screenWidth, int screenHeight){
    if (*playerCount >= max) return;
    player[*playerCount].pos = pos;
    float cellWidth  = (float)screenWidth / (float)COLS;
    float cellHeight = (float)screenHeight / (float)ROWS;

    player[*playerCount].basePos = (Vector2){ pos.x / ((float)screenWidth),
                                              pos.y / ((float)screenHeight) };
    float s = (cellWidth < cellHeight) ? cellWidth : cellHeight;
    player[*playerCount].size = s * 0.01562f;
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

void selectUnity(UnitType selected, int selTower, int screenWidth, int screenHeight){
    if (selTower >= 0 && selTower < towerCount && towers[selTower].active) {
        if (!towers[selTower].hasDefender) {
            switch (selected) {

                case UNIT_ARCHER:
                    if (ownedArchers > 0) {
                        AddPlayer ( archers, 
                                    (Vector2){ towers[selTower].pos.x + 5, towers[selTower].pos.y - 68 },
                                    MAX_ARCHERS, 
                                    &archerCount,
                                    screenWidth, 
                                    screenHeight );

                        towers[selTower].hasDefender = true;

                    } else printf("Você não tem arqueiros suficientes!\n");
                    
                break;

                case UNIT_WIZARD:
                    if (wizardUnlocked || ownedWizards > 0) {
                        AddPlayer ( wizards, 
                                    (Vector2){ towers[selTower].pos.x - 8, 
                                    towers[selTower].pos.y - 68 },
                                    MAX_WIZARDS, 
                                    &wizardCount,
                                    screenWidth, 
                                    screenHeight );

                        towers[selTower].hasDefender = true;
                        if (!wizardUnlocked) ownedWizards--;

                    } else printf("Você não tem magos suficientes!\n");
                    
                break;

                case UNIT_CANNON:
                    if (cannonUnlocked || ownedCannons > 0) {
                        AddPlayer ( cannons, 
                                    (Vector2){ towers[selTower].pos.x + 4, towers[selTower].pos.y - 28 },
                                    MAX_CANNONS, 
                                    &cannonCount,
                                    screenWidth, 
                                    screenHeight );

                        towers[selTower].hasDefender = true;
                        if (!cannonUnlocked) ownedCannons--;

                    } else printf("Você não tem canhões suficientes!\n");
                    
                break;
            }

        } else printf("Essa torre já tem um defensor!\n");
    }
}

void putTowerOnGrid(Vector2 mousePos, unsigned char *mapa, int screenWidth, int screenHeight){
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

    if(!IsTowerOnPath(gridPos, mapa)){
        if (!IsTowerOnGrid(gridPos)) {
            AddTower(cellCenter, screenWidth, screenHeight);
        } else {
            printf("Já existe uma torre nesse grid!\n");
        }
    }
}

void updatePlayers(Players *player, Projects *project, float dt, float attackRange, float frameTime, int i, int framesShot){
    player[i].frameTime += dt;
        if (player[i].frameTime >= frameTime) {
            player[i].frame = (player[i].frame + 1) % framesShot;
            player[i].frameTime = 0;
        }

        player[i].shotTimer -= dt;
        if (player[i].shotTimer <= 0) {
            bool foundTarget = false;

            for (int e = 0; e < MAX_ENEMIES; e++) {
                if (!enemies[e].active) continue;

                float dist = Vector2Distance(player[i].pos, enemies[e].pixelPos);

                if (dist <= attackRange) {
                    player[i].isShooting = true;
                    shootProject(project, player[i].pos, enemies[e].pixelPos, ARROW_SPEED, e, MAX_ARROWS, false);
                    player[i].shotTimer = ARCHER_SHOT_TIME;
                    foundTarget = true;
                    break;
                }
            }

                for (int e = 0; e < MAX_ORCS; e++) {
                    if (!orcs[e].active) continue;
                    float dist = Vector2Distance(player[i].pos, orcs[e].pos);
                    if (dist <= ATTACK_RANGE_ARCHER) {
                        player[i].isShooting = true;
                        shootProject(project, player[i].pos, orcs[e].pos, ARROW_SPEED, e, MAX_ARROWS, false);
                        player[i].shotTimer = 1.0f;
                        foundTarget = true;
                        break;
                    }
                }

            if (!foundTarget) player[i].isShooting = false;
        }
}

void updateProjects(Projects *project, bool hasFrames, float dt, float frameTime, int i, int damage, int quantFrames){
    if(hasFrames){
        project[i].frameTime += dt;
        if (project[i].frameTime >= frameTime) {
            project[i].frame = (project[i].frame + 1) % quantFrames;
            project[i].frameTime = 0;
        }
    }

    Vector2 dir = Vector2Normalize(Vector2Subtract(project[i].target, project[i].pos));
    project[i].pos = Vector2Add(project[i].pos, Vector2Scale(dir, project[i].speed * dt));
    if (Vector2Distance(project[i].pos, project[i].target) < 5.0f) {
        int e = project[i].enemyIndex;
        int x = project[i].orcIndex;
        if (e >= 0 && e < MAX_ENEMIES && enemies[e].active) {
            enemies[e].currentHealth -= damage;
            if (enemies[e].currentHealth <= 0) {
                    enemies[e].active = false;
                    playerGold += GOLD_FOR_ENEMY;
                }
            }
            if (x >= 0 && x < MAX_ORCS && orcs[x].active) {
                orcs[x].currentHealth -= damage;
                if (orcs[x].currentHealth <= 0) {
                    orcs[x].active = false;
                    playerGold += GOLD_FOR_ORC;
                }
            }
        project[i].active = false;
    }
}

void UpdatePlayer(unsigned char *mapa, int screenWidth, int screenHeight, int bonusArcherDamage, int bonusWizardDamage, int bonusCannonDamage, int penaltyArcherDamage, int penaltyWizardDamage, int penaltyCannonDamage)
{
    float dt = GetFrameTime();
    Vector2 mousePos = GetMousePosition();
    if (playerGold < 0)  playerGold = 0;

    //verifica se deve botar a torre no mapa
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !HUD_IsActive()) putTowerOnGrid(mousePos, mapa, screenWidth, screenHeight);

    //faz a hud aparecer
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && !HUD_IsActive()) {
        for (int i = 0; i < towerCount; i++) {
            if (towers[i].active && CheckCollisionPointCircle(mousePos, towers[i].pos, towers[i].size / 2)) {
                HUD_ShowAt(towers[i].pos, i);
                return;
            }
        }
    }

    //hud para adicionar personagem
    if (HUD_IsActive()) {
        HUD_Update(towers);
        UnitType selected = HUD_GetSelectedUnit();
        int selTower = HUD_GetSelectedTower();

        selectUnity(selected, selTower, screenWidth, screenHeight);
    }

    for (int i = 0; i < archerCount; i++) {
        if (!archers[i].active) continue;

        updatePlayers(archers, arrows, dt, ATTACK_RANGE_ARCHER, 0.1f, i, ARCHER_QT_FRAMES_SHOOT);
    }

    for (int i = 0; i < wizardCount; i++) {
        if (!wizards[i].active) continue;

        updatePlayers(wizards, fireballs, dt, ATTACK_RANGE_WIZARD, 0.12f, i, WIZARD_QT_FRAMES_SHOOT);
    }

    for (int i = 0; i < cannonCount; i++) {
        if (!cannons[i].active) continue;
        
        updatePlayers(cannons, cannonballs, dt, ATTACK_RANGE_CANNON, 0.1f, i, CANNON_QT_FRAMES_SHOOT);
    }

    for (int i = 0; i < MAX_ARROWS; i++) {
        if (!arrows[i].active) continue;
        
        updateProjects(arrows, false, dt, 1, i, ARROW_DAMAGE + bonusArcherDamage - penaltyArcherDamage, 1);
    }

    for (int i = 0; i < MAX_FIREBALLS; i++) {
        if (!fireballs[i].active) continue;
        updateProjects(fireballs, true, dt, 0.08f, i, FIREBALL_DAMAGE + bonusWizardDamage - penaltyWizardDamage, FIREBALL_QT_FRAMES);
    }

    for (int i = 0; i < MAX_CANNONBALLS; i++) {
        if (!cannonballs[i].active) continue;
        updateProjects(cannonballs, false, dt, 1, i, CANNONBALL_DAMAGE + bonusCannonDamage - penaltyCannonDamage, 1);
    }
}

void DrawTowers() {
    for (int i = 0; i < towerCount; i++) {
        if (!towers[i].active) continue;

        Rectangle src = { 0.0f, 0.0f, (float)torreTexture.width, (float)torreTexture.height };
        Rectangle dest = {
            towers[i].pos.x - towers[i].size / 2,
            towers[i].pos.y - towers[i].size / 2,
            towers[i].size,
            towers[i].size
        };
        Vector2 origin = { 0, 0 };
        DrawTexturePro(torreTexture, src, dest, origin, 0.0f, WHITE);
    }
}

void DrawPlayer(Players *player, Texture2D playerIdleTexture, Texture2D playerShootingTexture, int playerCount, int quantFrameShot, int quantFrameIdle){
    for(int i = 0; i < playerCount; i++){
        if (!player[i].active) continue;

        Texture2D tex;
        Rectangle src, dest;
        int frames;

        if (player[i].isShooting) {
            tex = playerShootingTexture;
            frames = quantFrameShot;
        } else {
            tex = playerIdleTexture;
            frames = quantFrameIdle;
        }

        int frameWidth  = tex.width  / frames;
        int frameHeight = tex.height;

        float w = frameWidth  * player[i].size;
        float h = frameHeight * player[i].size;

        float offset = h / 6.0f;
        
        src = (Rectangle){  frameWidth * player[i].frame, 
                            0, 
                            frameWidth, 
                            tex.height };

        dest = (Rectangle){ player[i].pos.x, 
                            player[i].pos.y - offset, 
                            w , 
                            h };
        
        Vector2 origin = { w / 2, h / 2 };

        DrawTexturePro( tex, 
                        src, 
                        dest, 
                        origin, 
                        0.0f, 
                        WHITE);
    }
}

void DrawPlayerInvertido(Players *player, Texture2D playerIdleTexture, Texture2D playerShootingTexture, int playerCount, int quantFrameShot, int quantFrameIdle){
    for(int i = 0; i < playerCount; i++){
        if (!player[i].active) continue;

        Texture2D tex;
        Rectangle src, dest;
        int frames;

        if (player[i].isShooting) {
            tex = playerShootingTexture;
            frames = quantFrameShot;
        } else {
            tex = playerIdleTexture;
            frames = quantFrameIdle;
        }

        int frameWidth  = tex.width  / frames;
        int frameHeight = tex.height;

        float w = frameWidth  * player[i].size;
        float h = frameHeight * player[i].size;

        float offset = h / 6.0f;
        
        src = (Rectangle){  frameWidth * player[i].frame, 
                            0, 
                            frameWidth * (-1), 
                            tex.height };

        dest = (Rectangle){ player[i].pos.x, 
                            player[i].pos.y - offset, 
                            w, 
                            h };
        
        Vector2 origin = { w / 2, h / 2 };

        DrawTexturePro( tex, 
                        src, 
                        dest, 
                        origin, 
                        0.0f, 
                        WHITE);
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

void resetTower(Tower *towers, int *towerCount, int maxTowers){
    for(int i = 0; i < maxTowers; i++){
        towers[i].active = false;
        towers[i].hasDefender = false;
    }
    *towerCount = 0;
}

void resetPlayer(Players *player, int *playerCount, int maxPlayer){
    
    for(int i = 0; i < maxPlayer; i++){
        player[i].active = false;
        player[i].basePos = (Vector2){0, 0};
        player[i].frame = 0;
        player[i].frameTime = 0;
        player[i].pos = (Vector2){0,0};
        player[i].shotTimer = 0;
        player[i].size = 0;
    }
    *playerCount = 0;
}

void resetProjects(Projects *project, int maxProjects){
    for(int i = 0; i < maxProjects; i++){
        project[i].active = false;
    }
}

void resetAll(){
    resetTower(towers, &towerCount, MAX_TOWERS);
    
    resetPlayer(archers, &archerCount, MAX_ARCHERS);
    resetPlayer(cannons, &cannonCount, MAX_CANNONS);
    resetPlayer(wizards, &wizardCount, MAX_WIZARDS);

    resetProjects(arrows, MAX_ARROWS);
    resetProjects(cannonballs, MAX_CANNONBALLS);
    resetProjects(fireballs, MAX_FIREBALLS);
}

void recenterPlayers(Players *player, int playerCount, int newWidth, int newHeight ){
    float scaleX = (float)newWidth;
    float scaleY = (float)newHeight;

    float cellWidth = newWidth / (float)COLS;
    float cellHeight = newHeight / (float)ROWS;
    float s = (cellWidth < cellHeight) ? cellWidth : cellHeight;
    
    for (int i = 0; i < playerCount; i++) {
        player[i].pos.x = player[i].basePos.x * scaleX;
        player[i].pos.y = player[i].basePos.y * scaleY;
        player[i].size = s * 0.01562f;
    }
}

void RecenterTowers(int newWidth, int newHeight)
{
    float scaleX = (float)newWidth ;
    float scaleY = (float)newHeight ;

    float cellWidth = newWidth / (float)COLS;
    float cellHeight = newHeight / (float)ROWS;

    for (int i = 0; i < towerCount; i++) {
        towers[i].pos.x = towers[i].basePos.x * scaleX;
        towers[i].pos.y = towers[i].basePos.y * scaleY;
        float s = (cellWidth < cellHeight) ? cellWidth : cellHeight;
        towers[i].size = s * 1.1f;

    }

    recenterPlayers(archers, archerCount, newWidth, newHeight);
    recenterPlayers(cannons, cannonCount, newWidth, newHeight);
    recenterPlayers(wizards, wizardCount, newWidth, newHeight);
    
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