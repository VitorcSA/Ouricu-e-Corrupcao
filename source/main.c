#include <raylib.h>

int main(){
    InitWindow(800, 450, "Poder e Corrupcao");
    SetTargetFPS(60);

    Texture2D titulo = LoadTexture("assets/titulo.png");

    int tamanhoFonte = 100;
    float espacos = 2;

    while (!WindowShouldClose()){
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawTexture(titulo,
                    (GetScreenWidth() - titulo.width) / 2,
                    (GetScreenHeight() - titulo.height) / 2,
                    WHITE);

        EndDrawing();
    }
    UnloadTexture(titulo);
    CloseWindow();

    return 0;
}