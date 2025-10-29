#include <raylib.h>
#include <math.h>

// Função que mostra e anima o título do jogo
void TelaTitulo(Texture2D titulo) {
    Vector2 posTitulo = { (GetScreenWidth() - titulo.width) / 2,
                          (GetScreenHeight() - titulo.height) / 2 };
    float alpha = 255.0f;        // Transparência inicial (visível)
    float fadeSpeed = 100.0f;    // Velocidade da transição
    float moveSpeed = 40.0f;     // Velocidade da subida
    bool iniciarTransicao = false;
    bool transicaoFeita = false;

    while (!WindowShouldClose() && !transicaoFeita) {

        // Quando o jogador aperta ENTER ou clique, inicia o fade
        if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            iniciarTransicao = true;
        }

        // Se a transição começou, move e diminui a transparência
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

        // Desenha o título
        DrawTexture(titulo, (int)posTitulo.x, (int)posTitulo.y,
                    (Color){255, 255, 255, (unsigned char)alpha});

        // 🔥 Texto piscando
        if (!iniciarTransicao) {
            // Calcula brilho do texto entre 0 e 1, oscilando com o tempo
            float brilho = (sinf(GetTime() * 3.0f) + 1.0f) / 2.0f; // Pisca ~3 vezes por segundo
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

    Texture2D titulo = LoadTexture("assets/titulo.png");

    // Chama a tela de título (animação)
    TelaTitulo(titulo);

    // Aqui começaria o jogo de verdade
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Jogo comeca aqui!", 300, 220, 20, RAYWHITE);
        EndDrawing();
    }

    UnloadTexture(titulo);
    CloseWindow();
    return 0;
}
