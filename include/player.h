#ifndef PLAYER_H
#define PLAYER_H
#include <raylib.h>

void InitPlayer(void);
void UpdatePlayer(void);
void DrawTowers(void);
void UnloadPlayer(void);
void RecenterTowers(int newWidth, int newHeight);

#endif