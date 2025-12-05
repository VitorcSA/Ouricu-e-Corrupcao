#include <raylib.h>
#include "game.h"
#include <stdio.h>
#include <enemies.h>
#include <HUD.h>
#include <string.h>
#include <stdlib.h>

float level = 0.0f;

static float timeBetweenWaves = 3.0f;   // intervalo entre hordas
static float waveCooldown = 0.0f;       // contador do cooldown

static int enemyIndex = 0;
int tempoPassado = 0;

EnemyWave *CreateWave(int number) {
    EnemyWave *w = malloc(sizeof(EnemyWave));

    w->number = number;
    w->enemiesToSpawn = 5 + number * 2; // cada horda tem mais inimigos
    w->enemiesSpawned = 0;

    w->spawnInterval = 1.0f - (number * 0.02f); // spawn mais rápido
    if (w->spawnInterval < 0.1f)
        w->spawnInterval = 0.1f;

    w->spawnTimer = 0;
    w->active = false;
    w->next = NULL;

    return w;
}

void CreateWaveList(WaveList *waves, int totalWaves) {
    waves->totalWaves = totalWaves;
    waves->head = NULL;
    waves->current = NULL;

    EnemyWave *prev = NULL;

    for (int i = 1; i <= totalWaves; i++) {
        EnemyWave *newWave = CreateWave(i);

        if (waves->head == NULL) {
            waves->head = newWave;
        } else {
            prev->next = newWave;
        }

        prev = newWave;
    }

    waves->current = waves->head;
}

void ResetWaveList(WaveList *waves) {
    EnemyWave *cur = waves->head;

    while (cur != NULL) {
        EnemyWave *next = cur->next;
        free(cur);
        cur = next;
    }

    waves->head = NULL;
    waves->current = NULL;
}

void StartWave(EnemyWave *w) {
    w->active = true;
    w->spawnTimer = 0;

    printf("Iniciando horda %d com %d inimigos\n",
           w->number, w->enemiesToSpawn);
}

void UpdateWaves(GameState *currentGameState, WaveList *waves, unsigned char *mapTower, float cellWidth, float cellHeight, float deltaTime) {

    EnemyWave *wave = waves->current;

    if (wave == NULL && TodosInimigosMortos(enemies, MAX_ENEMIES)) {
        printf("TODAS AS WAVES COMPLETAS! Voltando ao menu...\n");

        level += 0.5f;
        if (level > 6.0f) level = 6.0f;

        ResetWaveList(waves);
        CreateWaveList(waves, 3 + (int)level * 2);
        ResetEnemies(enemies, MAX_ENEMIES);
        resetAll();

        tempoPassado += 1;

        *currentGameState = WAVE_COMPLETE_STATE;
        return;
    }

    if (!wave->active) {
        waveCooldown += deltaTime;

        if (waveCooldown >= timeBetweenWaves &&
            TodosInimigosMortos(enemies, MAX_ENEMIES)) {

            waveCooldown = 0;
            StartWave(wave);
        }

        return;
    }

    wave->spawnTimer += deltaTime;

    if (wave->spawnTimer >= wave->spawnInterval &&
        wave->enemiesSpawned < wave->enemiesToSpawn) {

        SpawnEnemy(enemies, mapTower, cellWidth, cellHeight, wave->enemiesSpawned);

        wave->enemiesSpawned++;
        wave->spawnTimer = 0;
    }

    if (wave->enemiesSpawned == wave->enemiesToSpawn &&
    TodosInimigosMortos(enemies, MAX_ENEMIES)) {

        wave->active = false;

        printf("Horda %d concluída!\n", wave->number);

        // Ir para a próxima wave
        waves->current = wave->next;

        
    }
}

bool btnInvestimento(Vector2 mouseRight, Color corNormal, Color corHover, const char *tipoDeInvestimento, float *barra, int rightX, int by, int rightW, int rightH){
    Rectangle btnRight1 = { rightX, by, rightW, rightH };
    bool hovR1 = CheckCollisionPointRec(mouseRight, btnRight1);

    DrawRectangleRec(btnRight1, hovR1 ? corNormal : corHover);

    DrawRectangleLinesEx(btnRight1, 2, WHITE);

    char text[64] =  "Investir em ";
    snprintf(text, sizeof(text), "Investir em %s", tipoDeInvestimento);


    DrawText(text, btnRight1.x + 10, btnRight1.y + 15, 22, WHITE);

    int porcentagem = *barra * 100;
    DrawText(TextFormat("%d%%", porcentagem), btnRight1.x - 60, btnRight1.y + 15, 22, WHITE);

    return hovR1;
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
    
    int hudX = (GetScreenWidth() - goldHUD.width) / 2;
    int hudY = (GetScreenHeight() - goldHUD.height) / 2;

    DrawGoldHUDAt(&goldHUD, hudX - 100, hudY + 100);

    int priceArcher = 0;
    int priceWizard = 250;
    int priceCannon = 500;
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

    int rightX = screenWidth - (bw + 100);   // distância da borda direita
    int rightW = bw;
    int rightH = bh;

    Vector2 mouseRight = GetMousePosition();

    // Botão 1
    bool hovR1 = btnInvestimento(mouseRight, (Color){60,60,90,255}, (Color){40,40,60,255}, "Saude", barsaude,
                                   rightX, by, rightW, rightH );

    // Botão 2
    bool hovR2 = btnInvestimento(mouseRight, (Color){60,60,90,255}, (Color){40,40,60,255}, "Comida", barcomida,
                                   rightX, by + 90, rightW, rightH );

    // Botão 3
    bool hovR3 = btnInvestimento(mouseRight, (Color){60,60,90,255}, (Color){40,40,60,255}, "Poder", barpoder,
                                   rightX, by + 180, rightW, rightH );

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        float manterValor = 1.5f;
        if (hovR1){
            if(*playerGold >= 10 && *barsaude < 1.0f){
                *barsaude += 0.1f;
                *playerGold -= 10;

                if (*barsaude > 1.0f){
                    *barsaude = 1.0f;
                }
            }
        }

        if (hovR2){
            if(*playerGold >= 10 && *barcomida < 1.0f){
                *barcomida += 0.1f;
                *playerGold -= 10;
                
                if (*barcomida > 1.0f){
                    *barcomida = 1.0f;
                }
            }
        }

        if (hovR3){
            if(*playerGold >= 10 && *barpoder < 1.0f){
                *barpoder += 0.1f;
                *playerGold -= 10;

                if (*barpoder > 1.0f){
                    *barpoder = 1.0f;
                }
            }
        }
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