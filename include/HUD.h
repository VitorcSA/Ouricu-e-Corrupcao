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

extern float barsaude;
extern float barcomida;
extern float barinfra;

void HUD_ShowAt(Vector2 pos, int towerIndex);
void HUD_Update(void);
void HUD_Draw(void);
UnitType HUD_GetSelectedUnit(void);
bool HUD_IsActive(void);
int HUD_GetSelectedTower(void);

void InitGoldHUD(GoldHUD *hud);
void UpdateGoldHUD(GoldHUD *hud, int gold);
void DrawGoldHUDAt(GoldHUD *hud);

void SaveGold(int playerGold);
int LoadGold(void);

void InitRanking(void);
void RankingHUD(int screenHeight);

extern Texture2D cobreTexture;
extern Texture2D ferroTexture;
extern Texture2D bronzeTexture;
extern Texture2D prataTexture;
extern Texture2D ouroTexture;
extern Texture2D diamanteTexture;

void desenharRetangulo (int alturaImagem, int posYSprite);
void desenharRei(Texture2D reiTextura, int largura, int altura);
void DrawHorizontalBar(float x, float y, float width, float height, float value);
void DrawSideHUDBig(float v1, float v2, float v3);
void UpdateBars(int playerGold, int *prevGold);

#endif