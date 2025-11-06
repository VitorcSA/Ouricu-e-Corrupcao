#ifndef PLAYER_H
#define PLAYER_H
#include <raylib.h>

void InitPlayer(void);
void UpdatePlayer(void);
void DrawTowers(void);
void InitArchers(void);
void AddArcher(Vector2 pos);
void UpdateArchers(float dt);
void DrawArchers(void);
void UnloadArchers(void);
void UnloadPlayer(void);
void RecenterTowers(int newWidth, int newHeight);

#endif