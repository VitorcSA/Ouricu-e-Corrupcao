#ifndef HUD_H
#define HUD_H

#include <raylib.h>
#include <stdbool.h>
#include <game.h>

#define BTN_START_COR_HOVER                 (Color){0, 100, 0, 255}
#define BTN_START_COR                       DARKGREEN
#define BTN_START_LINES_COR                 GOLD
#define BTN_START_WIDTH                     220
#define BTN_START_HEIGHT                    60
#define BTN_START_X                         (screenWidth - BTN_START_WIDTH) / 2
#define BTN_START_Y                         fundoHeight - 120
#define BTN_START_FONTE                     30

#define BTN_LOJA_COR_HOVER                  (Color){20, 20, 30, 180}
#define BTN_LOJA_COR                        (Color){40, 40, 50, 200}
#define BTN_LOJA_LINES_COR                  (Color){255, 255, 255, 120}
#define BTN_LOJA_WIDTH                      180
#define BTN_LOJA_HEIGHT                     60
#define BTN_LOJA_X                          screenWidth - BTN_LOJA_WIDTH - 20
#define BTN_LOJA_Y                          20
#define BTN_LOJA_FONTE                      22

#define BTN_CONTINUAR_COR_HOVER             GRAY
#define BTN_CONTINUAR_COR                   DARKGRAY
#define BTN_CONTINUAR_LINES_COR             WHITE
#define BTN_CONTINUAR_WIDTH                 mw - 80
#define BTN_CONTINUAR_HEIGHT                50
#define BTN_CONTINUAR_X                     mx + 40
#define BTN_CONTINUAR_Y                     my + 90
#define BTN_CONTINUAR_FONTE                 26

#define BTN_VOLTAR_REINO_COR_HOVER          GRAY
#define BTN_VOLTAR_REINO_COR                DARKGRAY
#define BTN_VOLTAR_REINO_LINES_COR          WHITE
#define BTN_VOLTAR_REINO_WIDTH              mw - 80
#define BTN_VOLTAR_REINO_HEIGHT             50
#define BTN_VOLTAR_REINO_X                  mx + 40
#define BTN_VOLTAR_REINO_Y                  my + 160
#define BTN_VOLTAR_REINO_FONTE              26

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
    int width;
    int height;
} GoldHUD;

extern GoldHUD goldHUD;
extern Texture2D cobreTexture;
extern Texture2D ferroTexture;
extern Texture2D bronzeTexture;
extern Texture2D prataTexture;
extern Texture2D ouroTexture;
extern Texture2D diamanteTexture;

extern int hudHeight;
extern float barsaude;
extern float barcomida;
extern float barpoder;

UnitType HUD_GetSelectedUnit(void);
void HUD_ShowAt(Vector2 pos, int towerIndex);
void HUD_Update(Tower *tower);
void HUD_Draw(void);
bool HUD_IsActive(void);
int HUD_GetSelectedTower(void);

void InitGoldHUD(GoldHUD *hud);
void UpdateGoldHUD(GoldHUD *hud, int gold);
void DrawGoldHUDAt(GoldHUD *hud, float x, float y);

void InitRanking(void);
void RankingHUD(int screenHeight);
void writeLevel(int screenHeight);

void desenharReino(Texture2D reinoFundo, float fundoHeight, float scale, int screenWidth);
void desenharRetangulo(float fundoHeight, int screenWidth);
void desenharRei(Texture2D reiTextura, Vector2 posicaoRei, float fundoHeight, int screenWidth);
void criarBotao(GameState *currentGameState, GameState STATE, Color corBotao, Color corHover, Color RecLines, float fundoHeight,
    bool isRectangleRound, char *textoBotao, int screenWidth, int btnWidth, int btnHeight, int btnX, int btnY, int fonte);

void DrawHorizontalBar(float x, float y, float width, float height, float value);
void DrawSideHUDBig(float v1, float v2, float v3);
void UpdateBars(int playerGold, int *prevGold, int slot, bool diminuirBarras, Texture2D fundo, GameState *currentGameState, WaveList *waves);

#endif