#include <raylib.h>
#include <math.h>
#include "HUD.h"

void TelaTitulo(Texture2D titulo, Texture2D fundo) {
    Vector2 posTitulo = { (GetScreenWidth() - titulo.width) / 2,
                          (GetScreenHeight() - titulo.height) / 2 };
    float alpha = 255.0f;
    float fadeSpeed = 100.0f;
    float moveSpeed = 40.0f;
    bool iniciarTransicao = false;
    bool transicaoFeita = false;
    static bool borderless = false;

    while (!WindowShouldClose() && !transicaoFeita) {
        if (IsWindowResized()) {
            posTitulo.x = (GetScreenWidth() - titulo.width) / 2;
            posTitulo.y = (GetScreenHeight() - titulo.height) / 2;
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
        }

        if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            iniciarTransicao = true;
        }

        if (iniciarTransicao) {
            posTitulo.y -= moveSpeed * GetFrameTime();
            if (alpha > 0.0f) {
                alpha -= fadeSpeed * GetFrameTime();
                if (alpha <= 0.0f) {
                    alpha = 0.0f;
                    transicaoFeita = true;
                }
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexturePro(fundo,
                       (Rectangle){0, 0, fundo.width, fundo.height},
                       (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
                       (Vector2){0, 0}, 0.0f, WHITE);

        DrawTexture(titulo, (int)posTitulo.x, (int)posTitulo.y,
                    (Color){255, 255, 255, (unsigned char)alpha});

        if (!iniciarTransicao) {
            float brilho = (sinf(GetTime() * 3.0f) + 1.0f) / 2.0f;
            unsigned char intensidade = (unsigned char)(brilho * 255);

            DrawText("Pressione ENTER ou clique para continuar",
                     (GetScreenWidth() - MeasureText("Pressione ENTER ou clique para continuar", 20)) / 2,
                     GetScreenHeight() - 60, 20,
                     (Color){intensidade, intensidade, intensidade, 255});

            DrawText("[F11] alterna fullscreen", 10, 10, 20, GRAY);
        }

        EndDrawing();
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

    TelaTitulo(titulo, fundo);

    static bool borderless = false;

    while (!WindowShouldClose()) {
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

        BeginDrawing();
        ClearBackground(BLACK);

        DrawText("Jogo comeca aqui!",
                 (GetScreenWidth() - MeasureText("Jogo comeca aqui!", 20)) / 2,
                 GetScreenHeight() / 2, 20, RAYWHITE);

        DrawText("[F11] alterna fullscreen", 10, 10, 20, GRAY);

        desenharRetangulo(reiTextura.height);
        desenharRei(reiTextura, reiTextura.width, reiTextura.height);
        
        EndDrawing();
    }

    UnloadTexture(titulo);
    UnloadTexture(fundo);
    CloseWindow();
    return 0;
}