#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>

void InitPlayer(void);
void UpdatePlayer(void);
void DrawTowers(void);
void RecenterTowers(int newWidth, int newHeight);
void UnloadPlayer(void);
void InitArchers(void);
void AddArcher(Vector2 pos);
void UpdateArchers(float dt);
void DrawArchers(void);
void UnloadArchers(void);
void DrawArrows(void);
void UnloadArrows(void);
void AddWizard(Vector2 towerPos, float towerSize);
void DrawWizards(void);
void InitFireballs(void);
void ShootFireball(Vector2 start, Vector2 target, int enemyIndex);
void UpdateFireballs(float dt);
void DrawFireballs(void);
void UnloadFireballs(void);
void InitCannons(void);
void AddCannon(Vector2 towerPos, float towerSize);
void UpdateCannons(float dt);
void DrawCannons(void);
void UnloadCannons(void);

#endif