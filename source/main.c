#include <raylib.h>
#include <math.h>

void TelaTitulo(Texture2D titulo, Texture2D fundo) {
    Vector2 posTitulo = { (GetScreenWidth() - titulo.width) / 2,
                          (GetScreenHeight() - titulo.height) / 2 };
    float alpha = 255.0f;
    float fadeSpeed = 100.0f;
    float moveSpeed = 40.0f;
    bool iniciarTransicao = false;
    bool transicaoFeita = false;

    while (!WindowShouldClose() && !transicaoFeita) {

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
                     160, 400, 20, (Color){intensidade, intensidade, intensidade, 255});
        }

        EndDrawing();
    }
}

int main() {
    InitWindow(800, 450, "Poder e Corrupcao");
    SetTargetFPS(60);

    Texture2D fundo = LoadTexture("assets/fundotitulo.png");
    Texture2D titulo = LoadTexture("assets/titulo.png");
    Texture2D rei = LoadTexture("assets/rei.png");

    TelaTitulo(titulo, fundo);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Jogo comeca aqui!", 300, 220, 20, RAYWHITE);
        DrawTexture(rei,
            (int)((GetScreenHeight() - rei.height) / 8),
            (int)((GetScreenWidth() - rei.width) / 4),
            WHITE);
        EndDrawing();
    }

    UnloadTexture(titulo);
    UnloadTexture(fundo);
    CloseWindow();
    return 0;
}