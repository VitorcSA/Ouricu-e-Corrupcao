#include <raylib.h>
#include "HUD.h"
#include "player.h"
#include <stdio.h>
#include "game.h"
#include "telas.h"
#include "enemies.h"
#include "salvar.h"
#include <stdlib.h>

static bool hudVisible = false;
static Vector2 hudPos;
static int selectedTower = -1;
static UnitType chosenUnit = UNIT_NONE;
static int hudWidth = 150;
int hudHeight = 220;

float barsaude = 1.0f;
float barcomida = 1.0f;
float barpoder = 1.0f;

GoldHUD goldHUD;
Texture2D cobreTexture;
Texture2D ferroTexture;
Texture2D bronzeTexture;
Texture2D prataTexture;
Texture2D ouroTexture;
Texture2D diamanteTexture;

void InitRanking(){
    Image cobre = LoadImage("assets/Ranking/cobre.png");
    cobreTexture = LoadTextureFromImage(cobre);
    UnloadImage(cobre);

    Image ferro = LoadImage("assets/Ranking/ferro.png");
    ferroTexture = LoadTextureFromImage(ferro);
    UnloadImage(ferro);

    Image bronze = LoadImage("assets/Ranking/bronze.png");
    bronzeTexture = LoadTextureFromImage(bronze);
    UnloadImage(bronze);

    Image prata = LoadImage("assets/Ranking/prata.png");
    prataTexture = LoadTextureFromImage(prata);
    UnloadImage(prata);

    Image ouro = LoadImage("assets/Ranking/ouro.png");
    ouroTexture = LoadTextureFromImage(ouro);
    UnloadImage(ouro);

    Image diamante = LoadImage("assets/Ranking/diamante.png");
    diamanteTexture = LoadTextureFromImage(diamante);
    UnloadImage(diamante);
}

void HUD_ShowAt(Vector2 pos, int towerIndex) {
    hudVisible = true;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    hudWidth  = screenW * 0.18f;
    hudHeight = screenH * 0.30f;

    hudPos = (Vector2){ pos.x, 
                        pos.y - hudHeight * 0.5f };

    if (hudPos.x + hudWidth > screenW)  hudPos.x = screenW - hudWidth - 10;
    if (hudPos.y + hudHeight > screenH) hudPos.y = screenH - hudHeight - 10;
    if (hudPos.y < 0)                   hudPos.y = 10;
    if (hudPos.x < 0)                   hudPos.x = 10;

    selectedTower = towerIndex;

    chosenUnit = UNIT_NONE;
}

bool HUD_IsActive(void) { return hudVisible; }
int HUD_GetSelectedTower(void) { return selectedTower; }
UnitType HUD_GetSelectedUnit(void) { return chosenUnit; }

void HUD_Update(Tower *tower) {
    if (!hudVisible) return;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    hudWidth  = screenW * 0.18f;
    hudHeight = screenH * 0.30f;

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();

        int section = hudHeight / 3;
        Rectangle archerBtn = { hudPos.x, hudPos.y, hudWidth, section };
        Rectangle wizardBtn = { hudPos.x, hudPos.y + section, hudWidth, section };
        Rectangle cannonBtn = { hudPos.x, hudPos.y + 2 * section, hudWidth, section };
        Rectangle hud = { hudPos.x, hudPos.y, hudWidth, hudHeight };

        int index = HUD_GetSelectedTower();
        if (CheckCollisionPointRec(mouse, archerBtn) && !tower[index].hasDefender) {
            if(playerGold >= 20){
                chosenUnit = UNIT_ARCHER;
                playerGold -= 20;
                hudVisible = false;
            }
        }

        if (CheckCollisionPointRec(mouse, wizardBtn) && !tower[index].hasDefender) {
            if (wizardUnlocked && playerGold >= 30) {
                chosenUnit = UNIT_WIZARD;
                playerGold -= 30;
                hudVisible = false;
            }
        }
        else if (CheckCollisionPointRec(mouse, cannonBtn) && !tower[index].hasDefender) {
            if (cannonUnlocked && playerGold >= 40) {
                chosenUnit = UNIT_CANNON;
                playerGold -= 40;
                hudVisible = false;
            }
        }else if (!CheckCollisionPointRec(mouse, hud)) {
            hudVisible = false;
        }
    }
}

void HUD_Draw(void) {
    if (!hudVisible) return;

    DrawRectangleRec((Rectangle){hudPos.x, hudPos.y, hudWidth, hudHeight}, Fade(LIGHTGRAY, 0.95f));
    DrawRectangleLines(hudPos.x, hudPos.y, hudWidth, hudHeight, DARKGRAY);

    int section = hudHeight / 3;

    DrawRectangleLines(hudPos.x, hudPos.y, hudWidth, section, DARKGRAY);
    DrawRectangleLines(hudPos.x, hudPos.y + section, hudWidth, section, DARKGRAY);
    DrawRectangleLines(hudPos.x, hudPos.y + 2 * section, hudWidth, section, DARKGRAY);

    DrawText("Archer", hudPos.x + 10, hudPos.y + 10, 20, BLACK);
    DrawText("20 Gold", hudPos.x + 120, hudPos.y + 10, 20, YELLOW);

if (wizardUnlocked){
    DrawText("Wizard", hudPos.x + 10, hudPos.y + section + 10, 20, BLACK);
    DrawText("30 Gold", hudPos.x + 120, hudPos.y + section + 10, 20, YELLOW);
}
else{
    DrawText("Wizard (Bloqueado)", hudPos.x + 10, hudPos.y + section + 10, 20, RED);
}
if (cannonUnlocked){
    DrawText("Cannon", hudPos.x + 10, hudPos.y + 2*section + 10, 20, BLACK);
    DrawText("40 Gold", hudPos.x + 120, hudPos.y + 2*section + 10, 20, YELLOW);
}
else{
    DrawText("Cannon (Bloqueado)", hudPos.x + 10, hudPos.y + 2*section + 10, 20, RED);
}
}

void InitGoldHUD(GoldHUD *hud)
{
    hud->gold = 0;
    hud->rect.width = 140;
    hud->rect.height = 40;
    hud->rect.x = GetScreenWidth() - hud->rect.width - 10;
    hud->rect.y = GetScreenHeight() - hud->rect.height - 10;

    Image coinImg = LoadImage("assets/coin.png");
    ImageResize(&coinImg, 24, 24);
    hud->coinTexture = LoadTextureFromImage(coinImg);
    UnloadImage(coinImg);
}

void UpdateGoldHUD(GoldHUD *hud, int playerGold)
{
    hud->gold = playerGold;
}

void DrawGoldHUDAt(GoldHUD *hud, float x, float y) {
    float width = 220;
    float height = 60;

    DrawRectangleRounded((Rectangle){x, y, width, height}, 0.2f, 10, (Color){30, 30, 40, 180});
    DrawRectangleRoundedLines((Rectangle){x, y, width, height}, 0.2f, 10, Fade(GOLD, 0.6f));

    const int fontSize = 24;
    const char *label = "Gold";
    char goldText[32];
    sprintf(goldText, "%d", hud->gold);

    int goldLabelWidth = MeasureText(label, fontSize);
    int goldValueWidth = MeasureText(goldText, fontSize);
    int coinWidth = hud->coinTexture.width * 1.5f;
    int spacing = 10;

    float totalWidth = coinWidth + spacing + goldLabelWidth + spacing + goldValueWidth;
    if (totalWidth > width - 20) totalWidth = width - 20;

    float startX = x + 15;
    float centerY = y + (height - fontSize) / 2 + 2;

    float coinX = startX;
    float labelX = coinX + coinWidth + spacing;
    float valueX = labelX + goldLabelWidth + spacing;

    DrawTextureEx(
        hud->coinTexture,
        (Vector2){coinX, y + (height - hud->coinTexture.height * 1.5f) / 2},
        0.0f,
        1.5f,
        WHITE
    );

    DrawText(label, labelX, centerY, fontSize, YELLOW);
    DrawText(TextFormat("%d", playerGold), valueX, centerY, fontSize, RAYWHITE);

    if (hud->coinTexture.id > 0) {
        DrawTextureEx(
            hud->coinTexture,
            (Vector2){startX, y + (height - hud->coinTexture.height * 1.5f) / 2},
            0.0f,
            1.5f,
            WHITE
        );
    }
}

void desenharReino(Texture2D reinoFundo, float fundoHeight, float scale, int screenWidth){
    Rectangle source = { 0, 
                         0, 
                         (float)reinoFundo.width, 
                         (float)reinoFundo.height };

    Rectangle dest = { (screenWidth - reinoFundo.width * scale) / 2.0f,
                        (fundoHeight - reinoFundo.height * scale) / 2.0f,
                        reinoFundo.width * scale,
                        reinoFundo.height * scale };

    Vector2 origin = {0, 0};

    //imagem do reino no fundo
    DrawTexturePro( reinoFundo, 
                    source, 
                    dest, 
                    origin, 
                    0.0f, 
                    WHITE );
}

void desenharRetangulo(float fundoHeight, int screenWidth){
    //retangulo cinza do rei
    DrawRectangle ( 0, 
                    (int)fundoHeight, 
                    screenWidth, 
                    (int)hudHeight,
                    (Color){45, 45, 55, 255} );
            
    //linhas circurlando retangulo cinza
    DrawRectangleLines( 0, 
                        (int)fundoHeight, 
                        screenWidth, 
                        (int)hudHeight, 
                        DARKGRAY );
}

void desenharRei(Texture2D reiTextura, Vector2 posicaoRei, float fundoHeight, int screenWidth){
    //centralização do rei
    posicaoRei.x = (screenWidth - reiTextura.width) / 2.0f;
    posicaoRei.y = fundoHeight + 8;

    //sombra do rei
    DrawTexture( reiTextura, 
                 posicaoRei.x + 4, 
                 posicaoRei.y + 4, 
                 (Color){0, 0, 0, 80} );

    //desenho do rei
    DrawTexture( reiTextura, 
                 posicaoRei.x, 
                 posicaoRei.y, 
                 WHITE );
}

void criarBotao(GameState *currentGameState, GameState STATE, Color corBotao, Color corHover, Color RecLines, float fundoHeight, bool isRectangleRound,
                char *textoBotao, int screenWidth, int btnWidth, int btnHeight, int btnX, int btnY, int fonte){

    Rectangle botao = { btnX,
                        btnY,
                        btnWidth,
                        btnHeight };

    Vector2 mousePos = GetMousePosition();
    bool hover = CheckCollisionPointRec(mousePos, botao);

    if(!isRectangleRound){
        DrawRectangleRec(botao, hover ? corBotao : corHover);
        DrawRectangleLinesEx(botao, 3, RecLines);
    }else{
        DrawRectangleRounded(botao, 0.2f, 8, hover ? corBotao : corHover);
        DrawRectangleRoundedLines(botao, 0.2f, 8, RecLines);
    }

    int textoLargura = MeasureText(textoBotao, fonte);

    int textoX = botao.x + (botao.width - textoLargura) / 2;
    int textoY = botao.y + (botao.height - fonte) / 2 + 2;

    DrawText(  textoBotao,
                textoX,
                textoY, 
                fonte, 
                WHITE );

    if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) *currentGameState = STATE;

}

void DrawHorizontalBar(float x, float y, float width, float height, float value)
{
    if (value < 0) value = 0;
    if (value > 1) value = 1;

    Color col = (value > 0.66f) ? DARKGREEN :
                (value > 0.33f) ? ORANGE :
                                  RED;

    DrawRectangle(x, y, width, height, (Color){25,25,35,200});

    float filledWidth = width * value;
    DrawRectangle(x, y, filledWidth, height, col);
}

void DrawSideHUDBig(float v1, float v2, float v3)
{
    float barWidth  = 140;
    float barHeight = 24;
    float spacing   = 15;

    float hudWidth  = barWidth * 3 + spacing * 2 + 40;
    float hudHeight = barHeight + 40;

    float hudX = GetScreenWidth()  - hudWidth  - 20;
    float hudY = GetScreenHeight() - hudHeight - 20;

    DrawRectangle(hudX, hudY, hudWidth, hudHeight, (Color){15,15,20,200});

    float startX = hudX + 20;
    float startY = hudY + (hudHeight - barHeight)/2;

    DrawHorizontalBar(startX, startY, barWidth, barHeight, v1);
    DrawText("SAÚDE",startX + 40, startY + 3, 17, WHITE);
    DrawHorizontalBar(startX + barWidth + spacing, startY, barWidth, barHeight, v2);
    DrawText("COMIDA", startX + 190, startY + 3, 17, WHITE);
    DrawHorizontalBar(startX + 2*(barWidth + spacing), startY, barWidth, barHeight, v3);
    DrawText("PODER", startX + 350, startY + 3, 17, WHITE);
}

void UpdateBars(int playerGold, int *prevGold, int slot, bool diminuirBarras, Texture2D fundo, GameState *currentGameState, WaveList *waves) {

    if (playerGold > *prevGold) {
        *prevGold = playerGold;
    }

    int delta = *prevGold - playerGold;

    if (delta >= 10 && diminuirBarras) {
        int steps = delta / 10;

        barsaude -= steps * 0.05f;
                      
        barcomida -= steps * 0.08f;    
        
        barpoder -= steps * 0.02f;
                      
    }

    if (barsaude < 0.0f) barsaude = 0.0f;
    if (barsaude > 1.0f) barsaude = 1.0f;
    
    if (barcomida < 0.0f) barcomida = 0.0f;
    if (barcomida > 1.0f) barcomida = 1.0f;

    if (barpoder < 0.0f) barpoder = 0.0f;
    if (barpoder > 1.0f) barpoder = 1.0f;

    if(barsaude == 0 || barcomida == 0 || barpoder == 0){
        ResetWaveList(waves);
        ResetEnemies(enemies, MAX_ENEMIES);
        resetAll();
        DeleteSave(slot);
        TelaGameOver(fundo);
        *currentGameState = SAVE_STATE;
    }
    *prevGold = playerGold;
}

void RankingHUD(int screenHeight)
{
    int size = 192;
    Vector2 pos = {10, screenHeight - size - 10};
    Texture2D textures[] = {cobreTexture, ferroTexture, bronzeTexture, prataTexture, ouroTexture, diamanteTexture};

    int index = (int)level;  

    if (index >= 5) {
        index = 5;
    }

    // Desenha a textura correspondente ao nível
    DrawTexturePro(
        textures[index],
        (Rectangle){0, 0, textures[index].width, textures[index].height},
        (Rectangle){pos.x, pos.y, size, size},
        (Vector2){0, 0},
        0,
        WHITE
    );

}

void writeLevel(int screenHeight){
    int size = 192;
    Vector2 pos = {10, screenHeight - size - 10};
    float textX = pos.x + size + 10;
    float textY = pos.y + size/2 - 15;

    int nivel = (int)(level * 2) + 1;
    if(nivel > 12) nivel = 12;

    char *texto = malloc(32 * sizeof(char));

    if (nivel < 12)
        sprintf(texto, "NÍVEL %d", nivel);
    else
        sprintf(texto, "NÍVEL 12 - MAXIMO");

    DrawText(texto, textX, textY, 30, WHITE);

    free(texto);
}