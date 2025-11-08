#ifndef HUD_H
#define HUD_H

#include <raylib.h>
#include <stdbool.h>

typedef enum { 
    UNIT_NONE, 
    UNIT_ARCHER, 
    UNIT_WIZARD  // Novo defensor
} UnitType;

void HUD_ShowAt(Vector2 pos, int towerIndex);
void HUD_Update(void);
void HUD_Draw(void);
UnitType HUD_GetSelectedUnit(void);
bool HUD_IsActive(void);
int HUD_GetSelectedTower(void);

void desenharRetangulo (int alturaImagem, int posYSprite);
void desenharRei(Texture2D reiTextura, int largura, int altura);

#endif