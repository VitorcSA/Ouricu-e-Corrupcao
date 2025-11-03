#include <raylib.h>
#include <math.h>
#include "HUD.h"
#include "telaInicio.h"

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

    TelaLogo(logo);
    TelaTitulo(titulo, fundo);

    static bool borderless = false;
    const char *textoJogo = "Jogo comeca aqui!";

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

        if (IsWindowResized()) {
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawText(textoJogo,
                 (GetScreenWidth() - MeasureText(textoJogo, 20)) / 2,
                 GetScreenHeight() / 2, 20, RAYWHITE);

        DrawText("[F11] alterna fullscreen", 10, 10, 20, GRAY);

        desenharRetangulo(reiTextura.height);
        desenharRei(reiTextura, reiTextura.width, reiTextura.height);
        
        EndDrawing();
    }

    UnloadTexture(titulo);
    UnloadTexture(fundo);
    UnloadTexture(logo);
    CloseWindow();
    return 0;
}