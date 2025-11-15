#ifndef HUD_H
#define HUD_H

#include <raylib.h>
#include <stdbool.h>

typedef enum { 
    UNIT_NONE, 
    UNIT_ARCHER, 
    UNIT_WIZARD,
    UNIT_CANNON
} UnitType;

typedef struct {
    int gold;
    Rectangle rect;
    Texture2D coinTexture;
} GoldHUD;

void HUD_ShowAt(Vector2 pos, int towerIndex);
void HUD_Update(void);
void HUD_Draw(void);
void DrawGoldHUDAt(GoldHUD *hud);
UnitType HUD_GetSelectedUnit(void);
bool HUD_IsActive(void);
int HUD_GetSelectedTower(void);

void InitGoldHUD(GoldHUD *hud);
void UpdateGoldHUD(GoldHUD *hud, int gold);
void DrawGoldHUD(GoldHUD *hud);

void desenharRetangulo (int alturaImagem, int posYSprite);
void desenharRei(Texture2D reiTextura, int largura, int altura);
void DrawDefenderHUD(Texture2D torreImg, Texture2D archerImg, Texture2D wizardImg, Texture2D cannonImg,
                     int ownedTowers, int ownedArchers, int ownedWizards, int ownedCannons,
                     Vector2 reiPos, float fundoHeight);

#endif