#include <raylib.h>
#include <math.h>
#include "telaInicio.h"

void TelaLogo(Texture2D logo)
{
    float alpha = 0.0f;
    float fadeSpeed = 100.0f;
    float timer = 0.0f;
    int fase = 0;
    bool transicaoFeita = false;

    while (!WindowShouldClose() && !transicaoFeita)
    {
        float dt = GetFrameTime();

        switch (fase)
        {
            case 0:
                alpha += fadeSpeed * dt;
                if (alpha >= 255.0f)
                {
                    alpha = 255.0f;
                    fase = 1;
                }
                break;
            case 1:
                timer += dt;
                if (timer >= 1.0f)
                    fase = 2;
                break;
            case 2:
                alpha -= fadeSpeed * dt;
                if (alpha <= 0.0f)
                {
                    alpha = 0.0f;
                    transicaoFeita = true;
                }
                break;
        }

        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(logo,
                    (GetScreenWidth() - logo.width) / 2,
                    (GetScreenHeight() - logo.height) / 2,
                    (Color){255, 255, 255, (unsigned char)alpha});

        EndDrawing();
    }
}

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
            posTitulo.x = (GetScreenWidth() - titulo.width) / 2;
            posTitulo.y = (GetScreenHeight() - titulo.height) / 2;
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
                       (Vector2){0, 0}, 
                       0.0f, 
                       WHITE);

        DrawTexture(titulo, (int)posTitulo.x, (int)posTitulo.y,
                    (Color){255, 255, 255, (unsigned char)alpha});

        if (!iniciarTransicao) {
            float brilho = (sinf(GetTime() * 3.0f) + 1.0f) / 2.0f;
            unsigned char intensidade = (unsigned char)(brilho * 255);
            const char *texto = "Pressione ENTER ou clique para continuar";
            DrawText(texto,
                     (GetScreenWidth() - MeasureText(texto, 20)) / 2,
                     GetScreenHeight() - 60, 20,
                     (Color){intensidade, intensidade, intensidade, 255});
            DrawText("[F11] alterna fullscreen", 10, 10, 20, GRAY);
        }

        EndDrawing();
    }
}
