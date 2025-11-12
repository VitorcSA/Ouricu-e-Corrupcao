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
        "Use torres, barreiras e defensores para proteger seu povo.",
        "Clique com o botão esquerdo para adicionar torres.",
        "Clique com o botão direito para gerenciar defensores.",
        "Lembre-se: equilíbrio é essencial — defesa e recursos!",
        "GAME OVER se um dos lados for negligenciado!"
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
    Texture2D reinoFundo = LoadTexture("assets/reino.png");

    Vector2 posicaoRei = {
        (GetScreenWidth() - reiTextura.width) / 2,
        (GetScreenHeight() - reiTextura.height)
    };

    AtualizarCaminho(&pathStart, &pathEnd);
    TelaTitulo(titulo, fundo);

    static bool borderless = false;
    bool tutorialAtivo = true;
    bool jogoIniciado = false;

    const char *arquivoMapaTowerDefense = "assets/mapa/mapaTowerDefense";

    InitEnemies();
    InitPlayer();
    initTiles();
    InitGoldHUD(&goldHUD);

    if (!verificarSeMapaExiste(arquivoMapaTowerDefense)) {
        criadorDeMapa(arquivoMapaTowerDefense, 15, 15);
    }

    unsigned char *mapTower = ReadMap(arquivoMapaTowerDefense);
    if (!mapTower) {
        TraceLog(LOG_ERROR, "Falha ao carregar mapa!");
        CloseWindow();
        return 1;
    }

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
        ClearBackground((Color){20, 20, 30, 255});

        if (tutorialAtivo) {
            DrawTutorial();
            DrawText("Clique ou pressione ENTER para continuar",
                     (GetScreenWidth() - MeasureText("Clique ou pressione ENTER para continuar", 24)) / 2,
                     GetScreenHeight() - 80, 24, LIGHTGRAY);

            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                tutorialAtivo = false;
        }

        else if (!jogoIniciado) {
            float hudHeight = 220.0f;
            float fundoHeight = GetScreenHeight() - hudHeight;
            float scaleX = (float)GetScreenWidth() / reinoFundo.width;
            float scaleY = fundoHeight / reinoFundo.height;
            float scale = (scaleX > scaleY) ? scaleX : scaleY;

            Rectangle source = {0, 0, (float)reinoFundo.width, (float)reinoFundo.height};
            Rectangle dest = {
                (GetScreenWidth() - reinoFundo.width * scale) / 2.0f,
                (fundoHeight - reinoFundo.height * scale) / 2.0f,
                reinoFundo.width * scale,
                reinoFundo.height * scale
            };
            Vector2 origin = {0, 0};
            DrawTexturePro(reinoFundo, source, dest, origin, 0.0f, WHITE);

            DrawRectangle(0, (int)fundoHeight, GetScreenWidth(), (int)hudHeight,
                          (Color){45, 45, 55, 255});
            DrawRectangleLines(0, (int)fundoHeight, GetScreenWidth(), (int)hudHeight, DARKGRAY);

            posicaoRei.x = (GetScreenWidth() - reiTextura.width) / 2.0f;
            posicaoRei.y = fundoHeight + 8;
            DrawTexture(reiTextura, posicaoRei.x + 4, posicaoRei.y + 4, (Color){0, 0, 0, 80});
            DrawTexture(reiTextura, posicaoRei.x, posicaoRei.y, WHITE);
            int btnWidth = 220;
            int btnHeight = 60;
            Rectangle btnJogar = {
                (GetScreenWidth() - btnWidth) / 2,
                fundoHeight - 120,
                btnWidth,
                btnHeight
            };


            Vector2 mousePos = GetMousePosition();
            bool hover = CheckCollisionPointRec(mousePos, btnJogar);

            DrawRectangleRec(btnJogar, hover ? DARKGREEN : (Color){0, 100, 0, 255});
            DrawRectangleLinesEx(btnJogar, 3, GOLD);
            DrawText("JOGAR",
                     btnJogar.x + (btnWidth - MeasureText("JOGAR", 30)) / 2,
                     btnJogar.y + 12, 30, WHITE);

            DrawText("[F11] alterna fullscreen", 20, 20, 20, LIGHTGRAY);

            if ((hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) || IsKeyPressed(KEY_ENTER))
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
    UnloadTexture(reinoFundo);
    UnloadTexture(reiTextura);
    free(mapTower);
    UnloadPlayer();
    CloseWindow();
    return 0;
}
