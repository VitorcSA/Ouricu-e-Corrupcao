#include <raylib.h>
#include "game.h"
#include <stdio.h>
#include <enemies.h>
#include <HUD.h>

float level = 0.0f;

static float timeBetweenWaves = 3.0f;   // intervalo entre hordas
static float waveCooldown = 0.0f;       // contador do cooldown

static int enemyIndex = 0;

void StartNewWave(EnemyWave *wave) {
    wave->number++;
    wave->enemiesToSpawn = 5 + wave->number * 2;   // cada horda tem mais inimigos
    wave->enemiesSpawned = 0;
    wave->spawnInterval = 1.0f - (wave->number * 0.02f); // spawn mais rápido
    if (wave->spawnInterval < 0.1f) wave->spawnInterval = 0.1f;

    wave->spawnTimer = 0.0f;
    wave->active = true;

    printf("Iniciando horda %d com %d inimigos\n",
           wave->number, wave->enemiesToSpawn);
}

void ResetWaves(EnemyWave *wave) {
    wave->number = 0;
    wave->enemiesToSpawn = 0;
    wave->enemiesSpawned = 0;
    wave->spawnInterval = 0;
    wave->spawnTimer = 0;
    wave->active = false;

    wave->totalWaves = 3 + (int)level * 2; 
}

bool TodosInimigosMortos(Enemy *enemies, int maxEnemies) {
    for (int i = 0; i < maxEnemies; i++) {
        if (enemies[i].active) { 
            return false;
        }
    }
    return true; // nenhum ativo → todos mortos
}

void ResetEnemies(Enemy *enemies, int maxEnemies) {
    for (int i = 0; i < maxEnemies; i++) {
        enemies[i].active = false;
    }
}

void UpdateWaves(GameState *currentGameState, EnemyWave *wave, unsigned char *mapTower, float cellWidth, float cellHeight, float deltaTime) {

    if ((wave->number >= wave->totalWaves && !wave->active) && TodosInimigosMortos(enemies, MAX_ENEMIES)) {
        printf("TODAS AS WAVES COMPLETAS! Voltando ao menu...\n");

        level += 0.5f;
        if(level > 6.0f) level = 6.0f;

        ResetWaves(wave);
        ResetEnemies(enemies, MAX_ENEMIES);

        *currentGameState = MENU_STATE;
        return;
    }

    // Se a wave não está ativa, inicia o cooldown
    if (!wave->active) {
        waveCooldown += deltaTime;
        enemyIndex = 0;

        // Quando o cooldown acabar, inicia nova wave
        if (waveCooldown >= timeBetweenWaves) {
            waveCooldown = 0;
            StartNewWave(wave);
        }
        return;
    }

    // Atualiza timer interno da wave
    wave->spawnTimer += deltaTime;

    // Pode spawnar o próximo inimigo?
    if (wave->spawnTimer >= wave->spawnInterval
        && wave->enemiesSpawned < wave->enemiesToSpawn) {

        SpawnEnemy(enemies, mapTower, cellWidth, cellHeight, enemyIndex);
        enemyIndex += 1;
        printf("%d\n", enemyIndex); 
        wave->enemiesSpawned++;
        wave->spawnTimer = 0;
    }

    // Se todos foram spawnados e todos morreram → wave acabou
    if (wave->enemiesSpawned == wave->enemiesToSpawn ) {

        wave->active = false;

        printf("Horda %d concluída!\n", wave->number);
    }
}

void funlojaAtiva(GameState *currentGameState,float *barsaude, float *barcomida, float *barpoder, bool *cannonUnlocked, bool *wizardUnlocked, int *prevGold, int *ownedTowers, int *archerCount, int *playerGold, int screenWidth, int screenHeight){
    DrawRectangle ( 0, 
                    0, 
                    screenWidth, 
                    screenHeight, 
                    (Color){25,25,35,255} );

    DrawText( "LOJA", 
                (screenWidth - MeasureText("LOJA", 40)) / 2, 
                40, 
                40, 
                YELLOW );

    int priceArcher = 0;
    int priceWizard = 0;
    int priceCannon = 0;
    int priceTower = 10;

    int bx = 100;
    int by = 150;
    int bw = 260;
    int bh = 60;

    Vector2 mouseBuy = GetMousePosition();

    Rectangle btnBuyArcher = { bx, by, bw + 30, bh };
    bool hovAr = CheckCollisionPointRec(mouseBuy, btnBuyArcher);

    DrawRectangleRec(btnBuyArcher, hovAr ? DARKGREEN : GREEN);
    DrawRectangleLinesEx(btnBuyArcher, 2, BLACK);

    DrawText( "Arqueiro (Desbloqueado)", 
               btnBuyArcher.x + 10, 
                btnBuyArcher.y + 10, 
                22, 
                WHITE );

    if (hovAr && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (*playerGold >= priceArcher) {
            *playerGold -= priceArcher;
            *archerCount++;
        }
    }

    Rectangle btnBuyWizard = { bx, by + 90, bw + 30, bh };
    bool hovWiz = CheckCollisionPointRec(mouseBuy, btnBuyWizard);

    DrawRectangleRec(btnBuyWizard, hovWiz ? DARKBLUE : BLUE);
    DrawRectangleLinesEx(btnBuyWizard, 2, BLACK);

    if (!*wizardUnlocked) {
        DrawText( "Mago (Bloqueado)", 
                   btnBuyWizard.x + 10, 
                   btnBuyWizard.y + 10, 
                    22, 
                   WHITE );

        DrawText(TextFormat("Preço: %d", priceWizard), btnBuyWizard.x + 10, btnBuyWizard.y + 35, 20, YELLOW);

        if (hovWiz && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (*playerGold >= priceWizard) {
                *playerGold -= priceWizard;
                *wizardUnlocked = true;
            }
        }

    } else {
        DrawText( "Mago (Desbloqueado)", 
                   btnBuyWizard.x + 10, 
                   btnBuyWizard.y + 10, 
                   22, 
                   WHITE );
    }

    Rectangle btnBuyCannon = { bx, 
                                by + 180, 
                                bw + 
                                30, 
                                bh };

    bool hovCan = CheckCollisionPointRec(mouseBuy, btnBuyCannon);

    DrawRectangleRec(btnBuyCannon, hovCan ? DARKGRAY : GRAY);
    DrawRectangleLinesEx(btnBuyCannon, 2, BLACK);

    if (!*cannonUnlocked) {

        DrawText ( "Canhão (Bloqueado)", 
                    btnBuyCannon.x + 10, 
                    btnBuyCannon.y + 10, 
                    22, 
                    WHITE );

        DrawText ( TextFormat("Preço: %d", priceCannon), 
                    btnBuyCannon.x + 10, 
                    btnBuyCannon.y + 35, 
                    20, 
                    YELLOW );

        if (hovCan && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

            if (*playerGold >= priceCannon) {
                *playerGold -= priceCannon;
                *cannonUnlocked = true;
            }
        }

    } else {

        DrawText("Canhão (Desbloqueado)", btnBuyCannon.x + 10, btnBuyCannon.y + 10, 22, WHITE);

    }

    Rectangle btnBuyTower = { (screenWidth - bw) / 2,
                               by + 390,
                               bw,
                               bh };

    bool hovTor = CheckCollisionPointRec(mouseBuy, btnBuyTower);

    DrawRectangleRec(btnBuyTower, hovTor ? (Color){70,70,70,255} : (Color){100,100,100,255});
    DrawRectangleLinesEx(btnBuyTower, 2, BLACK);

    DrawText ( "Comprar Torre", 
                btnBuyTower.x + 10, 
                btnBuyTower.y + 10, 
                22, 
                WHITE );

    DrawText ( TextFormat("Preço: %d", priceTower), 
                btnBuyTower.x + 10, 
                btnBuyTower.y + 35, 
                20, 
                YELLOW );

    if (hovTor && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (*playerGold >= priceTower) {
            *playerGold -= priceTower;
            *ownedTowers += 1;
            UpdateBars(*playerGold, prevGold);
        }
    }

    DrawText ( TextFormat( "Torres: %d", *ownedTowers), 
                btnBuyTower.x + 85, 
                btnBuyTower.y + 20 + btnBuyTower.height + 10, 
                20, 
                WHITE );

    int rightX = screenWidth - (bw + 100);   // distância da borda direita
    int rightW = bw;
    int rightH = bh;

    Vector2 mouseRight = GetMousePosition();

    // Botão 1
    Rectangle btnRight1 = { rightX, by, rightW, rightH };
    bool hovR1 = CheckCollisionPointRec(mouseRight, btnRight1);
    DrawRectangleRec(btnRight1, hovR1 ? (Color){60,60,90,255} : (Color){40,40,60,255});
    DrawRectangleLinesEx(btnRight1, 2, WHITE);
    DrawText("Investir em Saúde", btnRight1.x + 10, btnRight1.y + 15, 22, WHITE);
    if(*barsaude > 1) barsaude = 1;
    int porcentagem = *barsaude * 100;
    DrawText(TextFormat("%d%%", porcentagem), btnRight1.x - 60, btnRight1.y + 15, 22, WHITE);

    // Botão 2
    Rectangle btnRight2 = { rightX, by + 90, rightW, rightH };
    bool hovR2 = CheckCollisionPointRec(mouseRight, btnRight2);
    DrawRectangleRec(btnRight2, hovR2 ? (Color){60,60,90,255} : (Color){40,40,60,255});
    DrawRectangleLinesEx(btnRight2, 2, WHITE);
    DrawText("Investir em Comida", btnRight2.x + 10, btnRight2.y + 15, 22, WHITE);
    int pct = *barcomida * 100;
    DrawText(TextFormat("%d%%", pct), btnRight2.x - 60, btnRight2.y + 15, 22, WHITE);

    // Botão 3
    Rectangle btnRight3 = { rightX, by + 180, rightW, rightH };
    bool hovR3 = CheckCollisionPointRec(mouseRight, btnRight3);
    DrawRectangleRec(btnRight3, hovR3 ? (Color){60,60,90,255} : (Color){40,40,60,255});
    DrawRectangleLinesEx(btnRight3, 2, WHITE);
    DrawText("Investir em Poder", btnRight3.x + 10, btnRight3.y + 15, 22, WHITE);
    int porcent = *barpoder * 100;
    DrawText(TextFormat("%d%%", porcent), btnRight3.x - 60, btnRight3.y + 15, 22, WHITE);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (hovR1) *barsaude += 0.1f;

        if (hovR2) *barcomida += 0.1f;
        
        if (hovR3) *barpoder += 0.1f;
    }

    Rectangle btnVoltar = { 40, 40, 160, 50 };
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, btnVoltar);

    DrawRectangleRec(btnVoltar, hover ? DARKGRAY : GRAY);
    DrawRectangleLinesEx(btnVoltar, 2, WHITE);

    DrawText ( "Voltar",
                btnVoltar.x + (btnVoltar.width - MeasureText("Voltar", 26)) / 2,
                btnVoltar.y + 12, 26, 
                WHITE );

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hover) *currentGameState = MENU_STATE;
}