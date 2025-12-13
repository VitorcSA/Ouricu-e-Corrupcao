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
#include "dialog.h"

void ResetTudo(WaveList *waves){
    resetEffects();
    CleanupEffects();
    ResetWaveList(waves);
    CreateWaveList(waves, 3);
    ResetEnemies(enemies, MAX_ENEMIES);
    resetAll();
}

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
    Texture2D logoText = LoadTextureFromImage(logo);
    UnloadImage(logo);
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    InitAudioDevice(); 

    Music music = LoadMusicStream("assets/cinematic-documentary-epic-dramatic-fallen-kingdom-short-ver-191717.mp3");
    SetMusicVolume(music, 0.5f); 
    PlayMusicStream(music);

    int prevGold = -1;
    extern int playerGold;

    WaveList waves;
    SaveData save;
    GameState currentGameState;
    Dialog *dialogo = NULL;

    //Iniciar Textura do rei
    Image rei = LoadImage("assets/rei.png");
    ImageResize(&rei, 200, 200);
    Texture2D reiTextura = LoadTextureFromImage(rei);
    UnloadImage(rei);

    //Iniciar outras texturas
    Texture2D fundo = LoadTexture("assets/fundotitulo.png");
    Texture2D titulo = LoadTexture("assets/titulo.png");
    Texture2D reinoFundo = LoadTexture("assets/reino.png");

    Vector2 posicaoRei;

    //Tela de inicio do reino
    //TelaLogo(logoText);
    TelaTitulo(titulo, fundo);
    InitPlayer();

    bool borderless = false;

    int slot = 4;
    vidaPortao = 3;
    currentGameState = SAVE_STATE;
    CreateWaveList(&waves, 3 + (int)level * 2);

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

    while (!WindowShouldClose() && slot != -1) {
        if (!IsMusicStreamPlaying(music)) {
            PlayMusicStream(music); // replay
        }
        UpdateMusicStream(music);
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
            if (dialogo != NULL) {
                free(dialogo);
                dialogo = NULL;
            }
            
            if(IsKeyPressed(KEY_ESCAPE)) currentGameState = SAVE_STATE;

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
            UpdateBars(playerGold, &prevGold, slot, false, fundo, &currentGameState, &waves);
        break;

        //Parte do jogo
        case GAME_STATE:
            
            if (IsKeyPressed(KEY_ESCAPE)) {
                currentGameState = PAUSE_STATE;
            }

            DrawMap(mapTower);
            drawLinesMap(mapTower);
            float dt = GetFrameTime();

            UpdateWaves(&currentGameState, &waves, mapTower, cellWidth, cellHeight, dt);

            UpdateEnemy2(enemies, mapTower, cellWidth, cellHeight, dt, &vidaPortao);
            UpdatePlayer(mapTower, screenWidth, screenHeight);

            UpdateGoldHUD(&goldHUD, playerGold);

            UpdateBars(playerGold, &prevGold, slot, true, fundo, &currentGameState, &waves);

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
            if(vidaPortao <= 0){
                ResetTudo(&waves);
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
            // Fundo escurecido com transparência
            DrawRectangle(0, 0, screenWidth, screenHeight, (Color){10, 10, 20, 220});
            vidaPortao = 3;

            // Card central
            float panelWidth  = screenWidth * 0.45f;
            float panelHeight = screenHeight * 0.40f;
            float panelX = (screenWidth  - panelWidth)  / 2.0f;
            float panelY = (screenHeight - panelHeight) / 2.0f;

            Rectangle panel = { panelX, panelY, panelWidth, panelHeight };

            // Fundo do card
            DrawRectangleRounded(panel, 0.18f, 16, (Color){25, 25, 60, 255});
            // Borda dourada
            DrawRectangleRoundedLines(panel, 0.18f, 16, (Color){255, 215, 0, 200});
          
            Rectangle topBar = { panelX, panelY, panelWidth, panelHeight * 0.22f };
            DrawRectangleRounded(topBar, 0.18f, 16, (Color){35, 40, 90, 255});

            // Título
            const char *msg = "Horda concluída!";
            int titleFont = 36;
            int titleWidth = MeasureText(msg, titleFont);
            int titleX = (int)(panelX + (panelWidth - titleWidth) / 2.0f);
            int titleY = (int)(panelY + panelHeight * 0.06f);

            DrawText(msg, titleX, titleY, titleFont, GOLD);

            char levelText[64];
            int levelAtual = (int)(level * 2)+1;
            snprintf(levelText, sizeof(levelText), "Level atual: %d", levelAtual);

            int infoFont = 24;
            int infoWidth = MeasureText(levelText, infoFont);
            int infoX = (int)(panelX + (panelWidth - infoWidth) / 2.0f);
            int infoY = (int)(panelY + panelHeight * 0.30f);

            DrawText(levelText, infoX, infoY, infoFont, RAYWHITE);

            // Mensagem explicando
            const char *sub1 = "Você completou todas as hordas deste ciclo.";
            const char *sub2 = "Recompensas já foram aplicadas ao reino.";

            int subFont = 20;
            int sub1Width = MeasureText(sub1, subFont);
            int sub2Width = MeasureText(sub2, subFont);

            int sub1X = (int)(panelX + (panelWidth - sub1Width) / 2.0f);
            int sub2X = (int)(panelX + (panelWidth - sub2Width) / 2.0f);

            int sub1Y = infoY + 40;
            int sub2Y = sub1Y + 28;

            DrawText(sub1, sub1X, sub1Y, subFont, (Color){200, 200, 220, 255});
            DrawText(sub2, sub2X, sub2Y, subFont, (Color){180, 200, 230, 255});

                    
            const char *press = "Pressione ENTER para voltar ao reino";

            float t = GetTime();
            unsigned char alpha = (unsigned char)(150 + 80 * sinf(t * 3.0f));
            Color pressColor = (Color){255, 255, 255, alpha};

            int pressFont = 22;
            int pressWidth = MeasureText(press, pressFont);
            int pressX = (int)(panelX + (panelWidth - pressWidth) / 2.0f);
            int pressY = (int)(panelY + panelHeight - 60);

            DrawText(press, pressX, pressY, pressFont, pressColor);

            waveCompleteTimer += GetFrameTime();

            if (IsKeyPressed(KEY_ENTER)) {
                waveCompleteTimer = 0.0f;
                currentGameState = DIALOGO_STATE;
            }
        } break;

        case DIALOGO_STATE:
            if (dialogo == NULL) {
                int id = GetRandomValue(1, MAX_NUM_PERGUNTAS);
                dialogo = carregarPergunta("dialogos.txt", id);
                
                if (dialogo) {
                    dialogo->active = true;
                } else {
                    // Se falhar ao carregar, pula direto pro jogo pra não travar
                    currentGameState = MENU_STATE; 
                }
            }

           if (dialogo != NULL && dialogo->active) {
                DrawDialogScreen(dialogo, screenWidth, screenHeight);
                int escolha = CheckDialogClick(dialogo, screenWidth, screenHeight);

                if (escolha != 0) {
                    printf("Opção %d selecionada:\n", escolha);
                    
                    ApplyDialogEffect(dialogo, escolha, &playerGold, &barsaude, &barcomida, &barpoder);

                    printf("teste\n");
                    free(dialogo);
                    dialogo = NULL;
                    currentGameState = MENU_STATE;
                }
                
            }

            break;

        }

        EndDrawing();
    }
    UnloadMusicStream(music);
    CloseAudioDevice();
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
