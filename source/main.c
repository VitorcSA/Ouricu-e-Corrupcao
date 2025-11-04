#include <raylib.h>
#include <math.h>
#include "HUD.h"
#include "telaInicio.h"
#include "enemies.h"

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
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].active) {
            enemies[i].pos.x = start.x + (end.x - start.x) * enemies[i].progress;
            enemies[i].pos.y = start.y + (end.y - start.y) * enemies[i].progress;
        }
    }
}

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Poder e Corrupcao");
    SetTargetFPS(60);

    Image rei = LoadImage("assets/rei.png");
    ImageResize(&rei, 200, 200);

    Texture2D fundo = LoadTexture("assets/fundotitulo.png");
    Texture2D titulo = LoadTexture("assets/titulo.png");
    Texture2D reiTextura = LoadTextureFromImage(rei);
    Texture2D logo = LoadTexture("assets/logo.png");

    Vector2 posicaoRei = {
        (GetScreenWidth() - reiTextura.width) / 2,
        (GetScreenHeight() - reiTextura.height) / 2
    };

    AtualizarCaminho(&pathStart, &pathEnd);
    TelaLogo(logo);
    TelaTitulo(titulo, fundo);

    static bool borderless = false;
    bool jogoIniciado = false;
    const char *textoJogo = "Pressione [ENTER] para iniciar o jogo!";

    InitEnemies();
    float enemyTimer = 0;

    while (!WindowShouldClose()) {
        if (IsWindowResized()) {
            posicaoRei.x = (GetScreenWidth() - reiTextura.width) / 2;
            posicaoRei.y = (GetScreenHeight() - reiTextura.height) / 2;
            AtualizarCaminho(&pathStart, &pathEnd);
            ReposicionarInimigos(pathStart, pathEnd);
        }

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
            AtualizarCaminho(&pathStart, &pathEnd);
            ReposicionarInimigos(pathStart, pathEnd);
        }

        if (IsKeyPressed(KEY_ENTER)) jogoIniciado = true;

        BeginDrawing();
        ClearBackground(BLACK);

        if (!jogoIniciado) {
            DrawText(textoJogo,
                     (GetScreenWidth() - MeasureText(textoJogo, 20)) / 2,
                     GetScreenHeight() / 2, 20, RAYWHITE);
            DrawText("[F11] alterna fullscreen", 10, 10, 20, GRAY);
            desenharRetangulo(reiTextura.height);
            desenharRei(reiTextura, posicaoRei.x, posicaoRei.y);
        } else {
            float dt = GetFrameTime();
            enemyTimer += dt;
            if (enemyTimer > 2.0f) {
                SpawnEnemy(pathStart);
                enemyTimer = 0;
            }
            UpdateEnemies(dt, pathStart, pathEnd);
            ClearBackground((Color){20, 20, 30, 255});
            DrawLineV(pathStart, pathEnd, GRAY);
            DrawEnemies();
            DrawText("Tower Defense - fase de inimigos", 10, 10, 20, WHITE);
        }

        EndDrawing();
    }

    UnloadTexture(titulo);
    UnloadTexture(fundo);
    UnloadTexture(logo);
    UnloadTexture(reiTextura);
    CloseWindow();
    return 0;
}