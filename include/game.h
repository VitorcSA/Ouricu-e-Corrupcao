#ifndef GAME_H
#define GAME_H

typedef struct {
    int number;             // número da horda
    int enemiesToSpawn;     // quantos inimigos essa horda terá
    int enemiesSpawned;     // quantos já foram spawnados
    float spawnInterval;    // tempo entre cada inimigo
    float spawnTimer;       // timer interno
    bool active;            // está acontecendo agora?
} EnemyWave;

extern EnemyWave wave;

void StartNewWave(Wave *wave);
void UpdateWaves(Wave *wave, unsigned char *mapTower, float cellWidth, float cellHeight, float deltaTime);

#endif