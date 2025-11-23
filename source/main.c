#include <raylib.h>
#include <math.h>
#include <stdlib.h>
#include "HUD.h"
#include "telaInicio.h"
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

    GoldHUD goldHUD;
    EnemyWave wave = {0};
    GameState currentGameState;

    playerGold = LoadGold();

    //Iniciar Textura do rei
    Image rei = LoadImage("assets/rei.png");
    ImageResize(&rei, 200, 200);
    Texture2D reiTextura = LoadTextureFromImage(rei);
    UnloadImage(rei);

    Texture2D fundo = LoadTexture("assets/fundotitulo.png");
    Texture2D titulo = LoadTexture("assets/titulo.png");
    Texture2D reinoFundo = LoadTexture("assets/reino.png");

    Vector2 posicaoRei;

    TelaTitulo(titulo, fundo);

    bool borderless = false;
    bool tutorialAtivo = true;
    bool jogoIniciado = false;
    bool pauseMenu = false;
    bool lojaAtiva = false;
    bool isGameOver = false;

    vidaPortao = 3;
    currentGameState = TUTORIAL_STATE;
    wave.totalWaves = 3;

    const char *arquivoMapaTowerDefense = "assets/mapa/mapaTowerDefense";

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
    InitPlayer();
    initTiles();
    InitGoldHUD(&goldHUD);
    InitRanking();

    while (!WindowShouldClose() && !isGameOver) {
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

        //Parte do tutorial
        case TUTORIAL_STATE:

            DrawTutorial();
            DrawText( "Clique ou pressione ENTER para continuar",
                      (screenWidth - MeasureText("Clique ou pressione ENTER para continuar", 24)) / 2,
                       screenHeight - 80, 
                       24, 
                       LIGHTGRAY );

            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                currentGameState = MENU_STATE;
            break;

        //Parte do reino
        case MENU_STATE:

            float hudHeight = 220.0f;
            float fundoHeight = screenHeight - hudHeight;
            float scaleX = (float)screenWidth / reinoFundo.width;
            float scaleY = fundoHeight / reinoFundo.height;
            float scale = (scaleX > scaleY) ? scaleX : scaleY;

            //desenho do reino no fundo
            desenharReino( reinoFundo, 
                           fundoHeight, 
                           scale, 
                           screenWidth );

            //desenho do retangulo atras do rei
            desenharRetangulo(fundoHeight, screenWidth);
            
            //desenho do rei
            desenharRei( reiTextura, 
                         posicaoRei, 
                         fundoHeight, 
                         screenWidth );

            //barra para cuidar do reino
            DrawSideHUDBig( barsaude, 
                            barcomida, 
                            barpoder );
                
            //nivel do jogador                
            RankingHUD(screenHeight);

            //botão para começar a partida
            botaoStart(&currentGameState, fundoHeight, screenWidth);

            //botao da loja
            botaoLoja(&lojaAtiva, screenWidth);

            //desenha o gold na tela
            DrawGoldHUDAt(&goldHUD);

            if (lojaAtiva) {

               funlojaAtiva(&barsaude, &barcomida, &barpoder, &lojaAtiva, &cannonUnlocked, &wizardUnlocked, &prevGold, &ownedTowers, &archerCount, &playerGold, screenWidth, screenHeight);
            }

            break;

        //Parte do jogo
        case GAME_STATE:

            if (IsKeyPressed(KEY_ESCAPE)) {
                pauseMenu = !pauseMenu;
            }

            if (pauseMenu) {

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

                DrawGoldHUDAt(&goldHUD);
                HUD_Draw();

                DrawRectangle(0, 0, screenWidth, screenHeight, (Color){0,0,0,180});

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

                Rectangle btnContinuar = { mx + 40, my + 90, mw - 80, 50 };
                Rectangle btnRei       = { mx + 40, my + 160, mw - 80, 50 };

                Vector2 mouse = GetMousePosition();

                bool hov1 = CheckCollisionPointRec(mouse, btnContinuar);
                bool hov2 = CheckCollisionPointRec(mouse, btnRei);

                DrawRectangleRec(btnContinuar, hov1 ? DARKGRAY : GRAY);
                DrawRectangleLinesEx(btnContinuar, 2, WHITE);
                DrawText ( "Continuar",
                            btnContinuar.x + (btnContinuar.width - MeasureText("Continuar", 26)) / 2,
                            btnContinuar.y + 12, 26, 
                            WHITE );

                DrawRectangleRec(btnRei, hov2 ? DARKGRAY : GRAY);
                DrawRectangleLinesEx(btnRei, 2, WHITE);
                DrawText ( "Voltar ao Reino",
                            btnRei.x + (btnRei.width - MeasureText("Voltar ao reino", 26)) / 2,
                            btnRei.y + 12, 26, 
                            WHITE );

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                    if (hov1) pauseMenu = false;

                    if (hov2) {
                        currentGameState = MENU_STATE;
                        pauseMenu = false;
                    }
                }

                EndDrawing();
                continue;
            }

            DrawMap(mapTower);
            drawLinesMap(mapTower);
            float dt = GetFrameTime();
    
            UpdateWaves(&wave, mapTower, cellWidth, cellHeight, dt);

            UpdateEnemy2(enemies, mapTower, cellWidth, cellHeight, dt, &vidaPortao);
            UpdatePlayer(mapTower);
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

            DrawGoldHUDAt(&goldHUD);
            HUD_Draw();
            if(vidaPortao <= 0){
                isGameOver = true;
            }

            break;
        }

        EndDrawing();
    }

    SaveGold(playerGold);
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