#include <raylib.h>

int main(){
    InitWindow(800, 450, "Poder e Corrupcao");

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawText("Poder e Corrupção", 350, 200, 20, BLACK);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}