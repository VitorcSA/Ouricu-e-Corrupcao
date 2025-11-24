#ifndef GAME_H
#define GAME_H


typedef struct {
    int number;             // número da horda
    int totalWaves;         // número total de waves do nível
    int enemiesToSpawn;     // quantos inimigos essa horda terá
    int enemiesSpawned;     // quantos já foram spawnados
    float spawnInterval;    // tempo entre cada inimigo
    float spawnTimer;       // timer interno
    bool active;            // está acontecendo agora?
} EnemyWave;

typedef enum {
    TUTORIAL_STATE,
    MENU_STATE,
    LOJA_STATE,
    GAME_STATE,
} GameState;    

void StartNewWave(EnemyWave *wave);
void UpdateWaves(EnemyWave *wave, unsigned char *mapTower, float cellWidth, float cellHeight, float deltaTime);
void funlojaAtiva(GameState *currentGameState,float *barsaude, float *barcomida, float *barpoder, bool *cannonUnlocked, bool *wizardUnlocked, int *prevGold, int *ownedTowers, int *archerCount, int *playerGold, int screenWidth, int screenHeight);

#endif