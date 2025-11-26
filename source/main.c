#include <raylib.h>
#include <math.h>
#include <stdlib.h>
#include "HUD.h"
#include "telas.h"
#include "enemies.h"
#include "player.h"
#include "mapa.h"
#include "criadorMapa.h"
#include "game.h"
#include "salvar.h"

void DrawTutorial(void) {
    const int fontSize = 24;
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int y = screenHeight / 6;

    DrawText("TUTORIAL", (screenWidth - MeasureText("TUTORIAL", 40)) / 2, y, 40, YELLOW);
    y += 70;

    const char *lines[] = {
    "Você é o rei e tem que defender seu reino de inimigos.",
    "Para isso você usará torres e defensores.",
    "Clique com o botão esquerdo em algum lugar no campo para adicionar uma torre.",
    "Clique com o botão direito na torre para abrir o menu de defensores.",
    "Escolha um defensor e posicione-o sobre a torre.",
    "Com o tempo você vai desbloqueando novos defensores.",
    "Cada defensor tem distâncias de ataque e danos diferentes.",
    "Lembre-se: ao gastar muitos recursos apenas em defesa, esquecendo de seu povo:",
    "GAME OVER"
    };

    int numLines = sizeof(lines) / sizeof(lines[0]);
    for (int i = 0; i < numLines; i++) {
        DrawText(lines[i],
                 (screenWidth - MeasureText(lines[i], fontSize)) / 2,
                 y,
                 fontSize,
                 WHITE);
        y += 40;
    }
}
float waveCompleteTimer = 0.0f;
int main() {
    //Configuração da janela
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Poder e Corrupcao");
    Image logo = LoadImage("assets/logo.png");
    SetWindowIcon(logo);
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    int prevGold = -1;
    extern int playerGold;

    EnemyWave wave = {0};
    SaveData save;
    GameState currentGameState;

    //Iniciar Textura do rei
    Image rei = LoadImage("assets/rei.png");
    ImageResize(&rei, 200, 200);
    Texture2D reiTextura = LoadTextureFromImage(rei);
    UnloadImage(rei);

    //Iniciar outras texturas
    Texture2D fundo = LoadTexture("assets/fundotitulo.png");
    Texture2D titulo = LoadTexture("assets/titulo.png");
    Texture2D reinoFundo = LoadTexture("assets/reino.png");

    //Ubucuabdi posição do rei
    Vector2 posicaoRei;

    //Tela de inicio do reino
    //TelaLogo(logo);
    TelaTitulo(titulo, fundo);
    InitPlayer();

    bool borderless = false;

    int slot = 4;
    vidaPortao = 3;
    currentGameState = SAVE_STATE;
    wave.totalWaves = 3 + level * 2;

    const char *arquivoMapaTowerDefense = "assets/mapa/mapaTowerDefense";
    char *textoBtnStart = "JOGAR";
    char *textoBtnLoja = "Loja";
    char *textoBtnContinuar = "Continuar";
    char *textoBtnVoltarReino = "Voltar ao reino";

    if (!verificarSeMapaExiste(arquivoMapaTowerDefense)) {
        criadorDeMapa(arquivoMapaTowerDefense, 15, 15);
    }

    unsigned char *mapTower = ReadMap(arquivoMapaTowerDefense);
    if (!mapTower) {
        TraceLog(LOG_ERROR, "Falha ao carregar mapa!");
        CloseWindow();
        return 1;
    }

    InitEnemiesTexture();
    InitEnemy(enemies, MAX_NORMAL_ENEMIES_HEALTH, MAX_ENEMIES);
    initTiles();
    InitGoldHUD(&goldHUD);
    InitRanking();

    while (!WindowShouldClose()) {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
        float cellWidth = screenWidth / (float)COLS;
        float cellHeight = screenHeight / (float)ROWS;
        
        if (IsWindowResized() || IsKeyPressed(KEY_F11)) {
            if (IsKeyPressed(KEY_F11)) {
                borderless = !borderless;

                if (borderless) {

                    SetWindowState(FLAG_WINDOW_UNDECORATED);
                    SetWindowSize(GetMonitorWidth(0), GetMonitorHeight(0));
                    SetWindowPosition(0, 0);

                } else {

                    ClearWindowState(FLAG_WINDOW_UNDECORATED);
                    SetWindowSize(1280, 720);
                    SetWindowPosition(
                        (GetMonitorWidth(0) - 1280) / 2,
                        (GetMonitorHeight(0) - 720) / 2
                    );

                }
            }

            int screenWidth = GetScreenWidth();
            int screenHeight = GetScreenHeight();
            float cellWidth = screenWidth / (float)COLS;
            float cellHeight = screenHeight / (float)ROWS;
        
            RecenterTowers(screenWidth, screenHeight);
            recenterEnemies(screenWidth, screenHeight);
        }
        BeginDrawing();
        ClearBackground((Color){20, 20, 30, 255});

        switch (currentGameState)
        {

        case SAVE_STATE:
            slot = saveSelection(&save, &barcomida, &barpoder, &barsaude, &level, &tempoPassado);
            currentGameState = TUTORIAL_STATE;

        //Parte do tutorial
        case TUTORIAL_STATE:

            DrawTutorial();
            DrawText( "Clique ou pressione ENTER para continuar",
                      (screenWidth - MeasureText("Clique ou pressione ENTER para continuar", 24)) / 2,
                       screenHeight - 80, 
                       24, 
                       LIGHTGRAY );

            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) currentGameState = MENU_STATE;

        break;

        //Parte do reino
        case MENU_STATE:
            vidaPortao = 3;

            float fundoHeight = screenHeight - hudHeight;
            float scaleX = (float)screenWidth / reinoFundo.width;
            float scaleY = fundoHeight / reinoFundo.height;
            float scale = (scaleX > scaleY) ? scaleX : scaleY;

            desenharReino( reinoFundo, 
                           fundoHeight, 
                           scale, 
                           screenWidth );

            desenharRetangulo(fundoHeight, screenWidth);
            
            desenharRei( reiTextura, 
                         posicaoRei, 
                         fundoHeight, 
                         screenWidth );

            DrawSideHUDBig( barsaude, 
                            barcomida, 
                            barpoder );
                
            RankingHUD(screenHeight);
            writeLevel(screenHeight);

            //botão para começar a partida
            criarBotao(&currentGameState, GAME_STATE, BTN_START_COR, BTN_START_COR_HOVER, BTN_START_LINES_COR, 
                        fundoHeight, false, textoBtnStart, screenWidth, BTN_START_WIDTH, BTN_START_HEIGHT, BTN_START_X, BTN_START_Y, BTN_START_FONTE);

            //botao da loja
            criarBotao(&currentGameState, LOJA_STATE, BTN_LOJA_COR, BTN_LOJA_COR_HOVER, BTN_LOJA_LINES_COR, 
                        fundoHeight, true, textoBtnLoja, screenWidth, BTN_LOJA_WIDTH, BTN_LOJA_HEIGHT, BTN_LOJA_X, BTN_LOJA_Y, BTN_LOJA_FONTE);

            DrawGoldHUDAt(&goldHUD, 20, 20);

        break;

        //Parte da loja
        case LOJA_STATE:
            funlojaAtiva(&currentGameState, &barsaude, &barcomida, &barpoder, &cannonUnlocked, &wizardUnlocked, &prevGold, &ownedTowers, &archerCount, &playerGold, screenWidth, screenHeight);
        break;

        //Parte do jogo
        case GAME_STATE:

            if (IsKeyPressed(KEY_ESCAPE)) {
                currentGameState = PAUSE_STATE;
            }

            DrawMap(mapTower);
            drawLinesMap(mapTower);
            float dt = GetFrameTime();

            UpdateWaves(&currentGameState, &wave, mapTower, cellWidth, cellHeight, dt);

            UpdateEnemy2(enemies, mapTower, cellWidth, cellHeight, dt, &vidaPortao);
            UpdatePlayer(mapTower, screenWidth, screenHeight);
            UpdateGoldHUD(&goldHUD, playerGold);

            ClearBackground((Color){20, 20, 30, 255});
            DrawEnemies2(enemies, walkTexture, MAX_ENEMIES);
            DrawTowers();

            DrawPlayerInvertido(archers, archerIdeleTexture, archerTexture, archerCount, ARCHER_QT_FRAMES_SHOOT, ARCHER_QT_FRAMES_IDLE);
            DrawPlayerInvertido(wizards, idlewizardTexture, wizardTexture, wizardCount, WIZARD_QT_FRAMES_SHOOT, WIZARD_QT_FRAMES_IDLE);
            DrawPlayer(cannons, cannonTextureIdle, cannonTextureShot, cannonCount, CANNON_QT_FRAMES_SHOOT, CANNON_QT_FRAMES_IDLE);

            drawProjects(arrows, arrowTexture, false, MAX_ARROWS, ARROW_QT_FRAMES);
            drawProjects(cannonballs, cannonballTexture, false, MAX_CANNONBALLS, CANNONBALL_QT_FRAMES);
            drawProjects(fireballs, fireballTexture, true, MAX_FIREBALLS, FIREBALL_QT_FRAMES);

            DrawSideHUDBig(barsaude, barcomida, barpoder);
            DrawGoldHUDAt(&goldHUD, 20, 20);
            HUD_Draw();
            UpdateBars(playerGold, &prevGold, slot, fundo, &currentGameState, &wave);
            if(vidaPortao <= 0){
                ResetWaves(&wave);
                ResetEnemies(enemies, MAX_ENEMIES);
                resetAll();
                DeleteSave(slot);
                TelaGameOver(fundo);
                currentGameState = SAVE_STATE;
            }

        break;

        case PAUSE_STATE:
            DrawMap(mapTower);
            drawLinesMap(mapTower);

            DrawEnemies2(enemies, walkTexture, MAX_ENEMIES);

            DrawTowers();
            DrawPlayerInvertido(archers, archerIdeleTexture, archerTexture, archerCount, ARCHER_QT_FRAMES_SHOOT, ARCHER_QT_FRAMES_IDLE);
            DrawPlayerInvertido(wizards, idlewizardTexture, wizardTexture, wizardCount, WIZARD_QT_FRAMES_SHOOT, WIZARD_QT_FRAMES_IDLE);
            DrawPlayer(cannons, cannonTextureIdle, cannonTextureShot, cannonCount, CANNON_QT_FRAMES_SHOOT, CANNON_QT_FRAMES_IDLE);

            drawProjects(arrows, arrowTexture, false, MAX_ARROWS, ARROW_QT_FRAMES);
            drawProjects(cannonballs, cannonballTexture, false, MAX_CANNONBALLS, CANNONBALL_QT_FRAMES);
            drawProjects(fireballs, fireballTexture, true, MAX_FIREBALLS, FIREBALL_QT_FRAMES);

            DrawGoldHUDAt(&goldHUD, 20, 20);
            HUD_Draw();

            //deixa parte atras do pause transparente cinza
            DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0,0,0,180});

            // --- caixa do botao de pause --- \\ 

            int mw = 380;
            int mh = 260;
            int mx = (screenWidth - mw) / 2;
            int my = (screenHeight - mh) / 2;

            DrawRectangle ( mx, 
                            my, 
                            mw, 
                            mh, 
                            (Color){30,30,40,240} );

            DrawRectangleLines( mx, 
                                my, 
                                mw, 
                                mh, 
                                GOLD );

            DrawText ( "PAUSADO", 
                        mx + (mw - MeasureText("PAUSADO", 32)) / 2,
                        my + 20, 
                        32, 
                        WHITE );

            // --- caixado botao de pause --- \\ 

            //botao continuar
            criarBotao(&currentGameState, GAME_STATE, BTN_CONTINUAR_COR, BTN_CONTINUAR_COR_HOVER, BTN_CONTINUAR_LINES_COR, 
                        fundoHeight, false, textoBtnContinuar, screenWidth, BTN_CONTINUAR_WIDTH, BTN_CONTINUAR_HEIGHT, BTN_CONTINUAR_X, BTN_CONTINUAR_Y, BTN_CONTINUAR_FONTE);

            //botao voltar ao reino
            criarBotao(&currentGameState, MENU_STATE, BTN_VOLTAR_REINO_COR, BTN_VOLTAR_REINO_COR_HOVER, BTN_VOLTAR_REINO_LINES_COR, 
                        fundoHeight, false, textoBtnVoltarReino, screenWidth, BTN_VOLTAR_REINO_WIDTH, BTN_VOLTAR_REINO_HEIGHT, BTN_VOLTAR_REINO_X, BTN_VOLTAR_REINO_Y, BTN_VOLTAR_REINO_FONTE);

        break;
        case WAVE_COMPLETE_STATE: {
            DrawRectangle(0, 0, screenWidth, screenHeight, (Color){20, 20, 30, 255});

            const char *msg = "Horda concluida";
            const char *sub = "Pressione ENTER para voltar ao reino";

            int msgFont = 40;
            int subFont = 24;

            int msgWidth = MeasureText(msg, msgFont);
            int subWidth = MeasureText(sub, subFont);

            DrawText(
                msg,
                (screenWidth - msgWidth) / 2,
                screenHeight / 2 - 40,
                msgFont,
                YELLOW
            );

            DrawText(
                sub,
                (screenWidth - subWidth) / 2,
                screenHeight / 2 + 20,
                subFont,
                LIGHTGRAY
            );

            waveCompleteTimer += GetFrameTime();

            if (IsKeyPressed(KEY_ENTER)) {
                waveCompleteTimer = 0.0f;
                currentGameState = MENU_STATE;
            }
        } break;

        }

        EndDrawing();
    }
    UpdateSave(&save, barcomida, barpoder, barsaude, level, tempoPassado);
    SaveGame(&save, slot);
    UnloadTexture(titulo);
    UnloadTexture(fundo);
    UnloadImage(logo);
    UnloadTexture(reinoFundo);
    UnloadTexture(reiTextura);
    free(mapTower);

    UnloadPlayer();
    CloseWindow();
    return 0;
}