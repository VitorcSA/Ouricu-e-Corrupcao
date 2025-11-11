#include <raylib.h>
#include <math.h>
#include <stdlib.h>
#include "HUD.h"
#include "telaInicio.h"
#include "enemies.h"
#include "player.h"
#include "mapa.h"
#include "criadorMapa.h"

Vector2 pathStart;
Vector2 pathEnd;

void AtualizarCaminho(Vector2 *start, Vector2 *end) {
    float centroY = GetScreenHeight() / 2.0f;
    float margem = GetScreenWidth() * 0.05f;
    start->x = margem;
    start->y = centroY;
    end->x = GetScreenWidth() - margem;
    end->y = centroY;
}

void ReposicionarInimigos(Vector2 start, Vector2 end) {
    float largura = end.x - start.x;
    float amplitude = 80.0f;
    float freq = 4.0f * PI / largura;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            float t = enemies[i].progress;
            enemies[i].pos.x = start.x + largura * t;
            enemies[i].pos.y = start.y + sinf((start.x + largura * t) * freq) * amplitude;
        }
    }
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
        "Para isso você usará torres, barreiras, e o mais importante: defensores.",
        "Clique com o botão esquerdo em algum lugar no campo para adicionar uma torre.",
        "Clique com o botão direito na torre para abrir o menu de defensores.",
        "Escolha um defensor e posicione-o sobre a torre.",
        "Com o tempo você vai desbloqueando novos defensores.",
        "Cada defensor tem habilidades diferentes que serão explicadas ao desbloqueá-los.",
        "Lembre-se: ao gastar muitos recursos apenas em defesa ou só em seu povo:",
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

    DrawText("Pressione [ENTER] para começar!",
             (screenWidth - MeasureText("Pressione [ENTER] para começar!", 24)) / 2,
             y + 40,
             24,
             GREEN);
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Poder e Corrupcao");
    SetTargetFPS(60);

    if (!FileExists("assets/rei.png")) {
        printf("Erro: assets/rei.png nao encontrado\n");
        CloseWindow();
        return 1;
    }

    int playerGold = 0;
    GoldHUD goldHUD;

    Image rei = LoadImage("assets/rei.png");
    ImageResize(&rei, 200, 200);
    Texture2D reiTextura = LoadTextureFromImage(rei);
    UnloadImage(rei);

    Texture2D fundo = LoadTexture("assets/fundotitulo.png");
    Texture2D titulo = LoadTexture("assets/titulo.png");
    Texture2D logo = LoadTexture("assets/logo.png");

    Vector2 posicaoRei = {
        (GetScreenWidth() - reiTextura.width) / 2,
        (GetScreenHeight() - reiTextura.height)
    };

    AtualizarCaminho(&pathStart, &pathEnd);
    TelaTitulo(titulo, fundo);

    static bool borderless = false;
    bool tutorialAtivo = true;
    bool jogoIniciado = false;
    const char *textoJogo = "Pressione [ENTER] para iniciar o jogo!";
    const char *arquivoMapaTowerDefense = "assets/mapa/mapaTowerDefense";

    InitEnemies();
    InitPlayer();
    initTiles();
    InitGoldHUD(&goldHUD);

    if (!verificarSeMapaExiste(arquivoMapaTowerDefense)) {
        criadorDeMapa(arquivoMapaTowerDefense, 15, 15);
    }

    unsigned char *mapTower = ReadMap(arquivoMapaTowerDefense);

    float enemyTimer = 0;

    while (!WindowShouldClose()) {
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

            posicaoRei.x = (GetScreenWidth() - reiTextura.width) / 2;
            posicaoRei.y = (GetScreenHeight() - reiTextura.height);
            AtualizarCaminho(&pathStart, &pathEnd);
            ReposicionarInimigos(pathStart, pathEnd);
            RecenterTowers(GetScreenWidth(), GetScreenHeight());
        }

        BeginDrawing();
        ClearBackground(BLACK);

        if (tutorialAtivo) {
            DrawTutorial();
            if (IsKeyPressed(KEY_ENTER)) tutorialAtivo = false;
        }

        else if (!jogoIniciado) {
            DrawText(textoJogo,
                     (GetScreenWidth() - MeasureText(textoJogo, 20)) / 2,
                     GetScreenHeight() / 2, 20, RAYWHITE);
            DrawText("[F11] alterna fullscreen", 10, 10, 20, GRAY);
            desenharRetangulo(reiTextura.height, posicaoRei.y);
            desenharRei(reiTextura, posicaoRei.x - 5, posicaoRei.y);
            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                jogoIniciado = true;
        }

        else {
            DrawMap(mapTower);
            drawLinesMap();
            float dt = GetFrameTime();
            enemyTimer += dt;
            if (enemyTimer > 2.0f) {
                SpawnEnemy(pathStart);
                enemyTimer = 0;
            }

            UpdateEnemies(dt, pathStart, pathEnd);
            ReposicionarInimigos(pathStart, pathEnd);
            UpdatePlayer();
            UpdateGoldHUD(&goldHUD, playerGold);

            ClearBackground((Color){20, 20, 30, 255});
            DrawEnemies();
            DrawTowers();
            DrawArchers();
            DrawWizards();
            DrawCannons();
            DrawFireballs();
            DrawArrows();
            DrawCannonballs();
            DrawGoldHUD(&goldHUD);
            DrawText("Tower Defense - fase de inimigos", 10, 10, 20, WHITE);
            HUD_Draw();
        }

        EndDrawing();
    }

    UnloadTexture(titulo);
    UnloadTexture(fundo);
    UnloadTexture(logo);
    UnloadTexture(reiTextura);
    free(mapTower);
    UnloadPlayer();
    CloseWindow();
    return 0;
}