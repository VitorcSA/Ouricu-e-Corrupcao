#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>

void InitPlayer(void);
void UpdatePlayer(void);
void DrawTowers(void);
void RecenterTowers(int newWidth, int newHeight);
void UnloadPlayer(void);

// --- Arqueiro ---
void InitArchers(void);
void AddArcher(Vector2 pos);
void UpdateArchers(float dt);
void DrawArchers(void);
void UnloadArchers(void);
void DrawArrows(void);
void UnloadArrows(void);

// --- Mago (Shot Wizard) ---
void AddWizard(Vector2 towerPos);
void DrawWizards(void);

#endif