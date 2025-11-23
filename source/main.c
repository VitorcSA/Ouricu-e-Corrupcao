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

            posicaoRei.x = (screenWidth - reiTextura.width) / 2;
            posicaoRei.y = (screenHeight - reiTextura.height);
        
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
            DrawTexturePro(reinoFundo, source, dest, origin, 0.0f, WHITE);

            //retangulo cinza do rei
            DrawRectangle ( 0, 
                           (int)fundoHeight, 
                            screenWidth, 
                            (int)hudHeight,
                            (Color){45, 45, 55, 255} );
            
            //linhas circurlando retangulo cinza
            DrawRectangleLines( 0, 
                                (int)fundoHeight, 
                                GetScreenWidth(), 
                                (int)hudHeight, 
                                DARKGRAY );
            
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
                         WHITE);

            //barra para cuidar do reino
            DrawSideHUDBig( barsaude, 
                            barcomida, 
                            barpoder );
                            
            RankingHUD(screenHeight);

            int btnWidth = 220;
            int btnHeight = 60;

            //botão para começar o jogo
            Rectangle btnJogar = { (GetScreenWidth() - btnWidth) / 2,
                                    fundoHeight - 120,
                                    btnWidth,
                                    btnHeight };

            Vector2 mousePos = GetMousePosition();
            bool hover = CheckCollisionPointRec(mousePos, btnJogar);

            DrawRectangleRec(btnJogar, hover ? DARKGREEN : (Color){0, 100, 0, 255});
            DrawRectangleLinesEx(btnJogar, 3, GOLD);
            DrawText( "JOGAR",
                      btnJogar.x + (btnWidth - MeasureText("JOGAR", 30)) / 2,
                      btnJogar.y + 12, 30, 
                      WHITE);

            DrawGoldHUDAt(&goldHUD);

            if (hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) currentGameState = GAME_STATE;

            float bw = 180;
            float bh = 60;

            Rectangle botaoLoja = { GetScreenWidth() - bw - 20,
                                    20,
                                    bw,
                                    bh };

            Vector2 mouseLoja = GetMousePosition();
            bool mouseSobreLoja = CheckCollisionPointRec(mouseLoja, botaoLoja);

            Color corBotaoLoja = mouseSobreLoja ?
                (Color){40, 40, 50, 200} :
                (Color){20, 20, 30, 180};

            Color corBordaLoja = (Color){255, 255, 255, 120};

            DrawRectangleRounded(botaoLoja, 0.2f, 8, corBotaoLoja);
            DrawRectangleRoundedLines(botaoLoja, 0.2f, 8, corBordaLoja);

            const char *textoBotaoLoja = "Loja";
            int fontSizeLoja = 22;
            int textoLarguraLoja = MeasureText(textoBotaoLoja, fontSizeLoja);

            int textoXLoja = botaoLoja.x + (botaoLoja.width - textoLarguraLoja) / 2;
            int textoYLoja = botaoLoja.y + (botaoLoja.height - fontSizeLoja) / 2 + 2;

            DrawText( textoBotaoLoja, 
                      textoXLoja, 
                      textoYLoja, 
                      fontSizeLoja, 
                      WHITE );

            if (mouseSobreLoja && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) lojaAtiva = true;

            else if (lojaAtiva) {

                DrawRectangle ( 0, 
                                0, 
                                screenWidth, 
                                screenHeight, 
                                (Color){25,25,35,255} );

                DrawText( "LOJA", 
                          (screenWidth - MeasureText("LOJA", 40)) / 2, 
                          40, 
                          40, 
                          YELLOW );

                int priceArcher = 0;
                int priceWizard = 0;
                int priceCannon = 0;
                int priceTower = 10;

                int bx = 100;
                int by = 150;
                int bw = 260;
                int bh = 60;

                Vector2 mouseBuy = GetMousePosition();

                Rectangle btnBuyArcher = { bx, by, bw + 30, bh };
                bool hovAr = CheckCollisionPointRec(mouseBuy, btnBuyArcher);

                DrawRectangleRec(btnBuyArcher, hovAr ? DARKGREEN : GREEN);
                DrawRectangleLinesEx(btnBuyArcher, 2, BLACK);

                DrawText( "Arqueiro (Desbloqueado)", 
                           btnBuyArcher.x + 10, 
                           btnBuyArcher.y + 10, 
                           22, 
                           WHITE );

                if (hovAr && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (playerGold >= priceArcher) {
                        playerGold -= priceArcher;
                        archerCount++;
                    }
                }

                Rectangle btnBuyWizard = { bx, by + 90, bw + 30, bh };
                bool hovWiz = CheckCollisionPointRec(mouseBuy, btnBuyWizard);

                DrawRectangleRec(btnBuyWizard, hovWiz ? DARKBLUE : BLUE);
                DrawRectangleLinesEx(btnBuyWizard, 2, BLACK);

                if (!wizardUnlocked) {
                    DrawText( "Mago (Bloqueado)", 
                               btnBuyWizard.x + 10, 
                               btnBuyWizard.y + 10, 
                               22, 
                               WHITE );

                    DrawText(TextFormat("Preço: %d", priceWizard), btnBuyWizard.x + 10, btnBuyWizard.y + 35, 20, YELLOW);

                    if (hovWiz && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if (playerGold >= priceWizard) {
                            playerGold -= priceWizard;
                            wizardUnlocked = true;
                        }
                    }

                } else {
                    DrawText( "Mago (Desbloqueado)", 
                               btnBuyWizard.x + 10, 
                               btnBuyWizard.y + 10, 
                               22, 
                               WHITE );
                }

                Rectangle btnBuyCannon = { bx, 
                                            by + 180, 
                                            bw + 
                                            30, 
                                            bh };

                bool hovCan = CheckCollisionPointRec(mouseBuy, btnBuyCannon);

                DrawRectangleRec(btnBuyCannon, hovCan ? DARKGRAY : GRAY);
                DrawRectangleLinesEx(btnBuyCannon, 2, BLACK);

                if (!cannonUnlocked) {

                    DrawText ( "Canhão (Bloqueado)", 
                                btnBuyCannon.x + 10, 
                                btnBuyCannon.y + 10, 
                                22, 
                                WHITE );

                    DrawText ( TextFormat("Preço: %d", priceCannon), 
                                btnBuyCannon.x + 10, 
                                btnBuyCannon.y + 35, 
                                20, 
                                YELLOW );

                    if (hovCan && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                        if (playerGold >= priceCannon) {
                            playerGold -= priceCannon;
                            cannonUnlocked = true;
                        }

                    }

                } else {

                    DrawText("Canhão (Desbloqueado)", btnBuyCannon.x + 10, btnBuyCannon.y + 10, 22, WHITE);

                }

                Rectangle btnBuyTower = { (screenWidth - bw) / 2,
                                           by + 390,
                                           bw,
                                           bh };

                bool hovTor = CheckCollisionPointRec(mouseBuy, btnBuyTower);

                DrawRectangleRec(btnBuyTower, hovTor ? (Color){70,70,70,255} : (Color){100,100,100,255});
                DrawRectangleLinesEx(btnBuyTower, 2, BLACK);

                DrawText ( "Comprar Torre", 
                            btnBuyTower.x + 10, 
                            btnBuyTower.y + 10, 
                            22, 
                            WHITE );

                DrawText ( TextFormat("Preço: %d", priceTower), 
                            btnBuyTower.x + 10, 
                            btnBuyTower.y + 35, 
                            20, 
                            YELLOW );

                if (hovTor && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (playerGold >= priceTower) {
                        playerGold -= priceTower;
                        ownedTowers++;
                        UpdateBars(playerGold, &prevGold);
                    }
                }

                DrawText ( TextFormat( "Torres: %d", ownedTowers), 
                            btnBuyTower.x + 85, 
                            btnBuyTower.y + 20 + btnBuyTower.height + 10, 
                            20, 
                            WHITE );

                int rightX = screenWidth - (bw + 100);   // distância da borda direita
                int rightW = bw;
                int rightH = bh;

                Vector2 mouseRight = GetMousePosition();

                // Botão 1
                Rectangle btnRight1 = { rightX, by, rightW, rightH };
                bool hovR1 = CheckCollisionPointRec(mouseRight, btnRight1);
                DrawRectangleRec(btnRight1, hovR1 ? (Color){60,60,90,255} : (Color){40,40,60,255});
                DrawRectangleLinesEx(btnRight1, 2, WHITE);
                DrawText("Investir em Saúde", btnRight1.x + 10, btnRight1.y + 15, 22, WHITE);
                int porcentagem = barsaude * 100;
                DrawText(TextFormat("%d%%", porcentagem), btnRight1.x - 60, btnRight1.y + 15, 22, WHITE);

                // Botão 2
                Rectangle btnRight2 = { rightX, by + 90, rightW, rightH };
                bool hovR2 = CheckCollisionPointRec(mouseRight, btnRight2);
                DrawRectangleRec(btnRight2, hovR2 ? (Color){60,60,90,255} : (Color){40,40,60,255});
                DrawRectangleLinesEx(btnRight2, 2, WHITE);
                DrawText("Investir em Comida", btnRight2.x + 10, btnRight2.y + 15, 22, WHITE);
                int pct = barcomida * 100;
                DrawText(TextFormat("%d%%", pct), btnRight2.x - 60, btnRight2.y + 15, 22, WHITE);

                // Botão 3
                Rectangle btnRight3 = { rightX, by + 180, rightW, rightH };
                bool hovR3 = CheckCollisionPointRec(mouseRight, btnRight3);
                DrawRectangleRec(btnRight3, hovR3 ? (Color){60,60,90,255} : (Color){40,40,60,255});
                DrawRectangleLinesEx(btnRight3, 2, WHITE);
                DrawText("Investir em Poder", btnRight3.x + 10, btnRight3.y + 15, 22, WHITE);
                int porcent = barpoder * 100;
                DrawText(TextFormat("%d%%", porcent), btnRight3.x - 60, btnRight3.y + 15, 22, WHITE);

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    if (hovR1) barsaude += 0.1f;

                    if (hovR2) barcomida += 0.1f;
                    
                    if (hovR3) barpoder += 0.1f;
                }

                Rectangle btnVoltar = { 40, 40, 160, 50 };
                Vector2 mouse = GetMousePosition();
                bool hover = CheckCollisionPointRec(mouse, btnVoltar);

                DrawRectangleRec(btnVoltar, hover ? DARKGRAY : GRAY);
                DrawRectangleLinesEx(btnVoltar, 2, WHITE);

                DrawText ( "Voltar",
                            btnVoltar.x + (btnVoltar.width - MeasureText("Voltar", 26)) / 2,
                            btnVoltar.y + 12, 26, 
                            WHITE );

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hover) lojaAtiva = false;

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