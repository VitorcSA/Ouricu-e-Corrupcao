#ifndef GAME_H
#define GAME_H

#include "player.h"

typedef struct EnemyWave{
    int number;             // número da horda
    int enemiesToSpawn;     // quantos inimigos essa horda terá
    int enemiesSpawned;     // quantos já foram spawnados
    float spawnInterval;    // tempo entre cada inimigo
    float spawnTimer;       // timer interno
    bool active;            // está acontecendo agora?

    struct EnemyWave *next; // próxima wave
} EnemyWave;

typedef struct {
    EnemyWave *head;
    EnemyWave *current;
    int totalWaves;
} WaveList;

typedef enum {
    TUTORIAL_STATE,
    MENU_STATE,
    LOJA_STATE,
    GAME_STATE,
    PAUSE_STATE,
    SAVE_STATE,
    WAVE_COMPLETE_STATE,
} GameState;    

extern float level;
extern int tempoPassado;

void CreateWaveList(WaveList *waves, int totalWaves);
void ResetWaveList(WaveList *waves);
void StartWave(EnemyWave *w);

void UpdateWaves(GameState *currentGameState,
                 WaveList *waves,
                 unsigned char *mapTower,
                 float cellWidth,
                 float cellHeight,
                 float deltaTime);

void funlojaAtiva(GameState *currentGameState,float *barsaude, float *barcomida, float *barpoder, bool *cannonUnlocked, 
                    bool *wizardUnlocked, int *prevGold, int *ownedTowers, int *archerCount, int *playerGold, int screenWidth, int screenHeight);

#endif