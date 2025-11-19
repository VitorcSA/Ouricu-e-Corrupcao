#include <raylib.h>
#include "game.h"
#include <stdio.h>
#include <enemies.h>

EnemyWave enemywave = {0};
float timeBetweenWaves = 3.0f;   // intervalo entre hordas
float waveCooldown = 0.0f;       // contador do cooldown

void StartNewWave(EnemyWave *wave) {
    wave->number++;
    wave->enemiesToSpawn = 5 + wave->number * 2;   // cada horda tem mais inimigos
    wave->enemiesSpawned = 0;
    wave->spawnInterval = 0.5f - (wave->number * 0.02f); // spawn mais rápido
    if (wave->spawnInterval < 0.1f) wave->spawnInterval = 0.1f;

    wave->spawnTimer = 0.0f;
    wave->active = true;

    printf("Iniciando horda %d com %d inimigos\n",
           wave->number, wave->enemiesToSpawn);
}

void UpdateWaves(EnemyWave *wave, unsigned char *mapTower, float cellWidth, float cellHeight, float deltaTime) {

    // Se a wave não está ativa, inicia o cooldown
    if (!wave->active) {
        waveCooldown += deltaTime;

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

        SpawnEnemy(enemies, mapTower, cellWidth, cellHeight); 
        wave->enemiesSpawned++;
        wave->spawnTimer = 0;
    }

    // Se todos foram spawnados e todos morreram → wave acabou
    if (wave->enemiesSpawned == wave->enemiesToSpawn ) {

        wave->active = false;

        printf("Horda %d concluída!\n", wave->number);
    }
}
