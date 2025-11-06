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

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 720, "Poder e Corrupcao");
    SetTargetFPS(60);

    // --- VERIFICAÇÕES E SEGURANÇA ADICIONADAS AQUI ---
    if (!FileExists("assets/rei.png")) {
        printf("Erro: assets/rei.png nao encontrado\n");
        CloseWindow();
        return 1;
    }

    Image rei = LoadImage("assets/rei.png");
    if (rei.data == NULL) {
        printf("Erro: falha ao carregar rei.png\n");
        CloseWindow();
        return 1;
    }

    ImageResize(&rei, 200, 200);
    Texture2D reiTextura = LoadTextureFromImage(rei);
    UnloadImage(rei);
    if (reiTextura.id == 0) {
        printf("Erro: falha ao criar textura do rei\n");
        CloseWindow();
        return 1;
    }

    if (!FileExists("assets/fundotitulo.png") || !FileExists("assets/titulo.png") || !FileExists("assets/logo.png")) {
        printf("Erro: algum asset de titulo/logo/fundo esta faltando em assets/\n");
        UnloadTexture(reiTextura);
        CloseWindow();
        return 1;
    }

    Texture2D fundo = LoadTexture("assets/fundotitulo.png");
    Texture2D titulo = LoadTexture("assets/titulo.png");
    Texture2D logo = LoadTexture("assets/logo.png");

    if (fundo.id == 0 || titulo.id == 0 || logo.id == 0) {
        printf("Erro: falha ao carregar uma das texturas de titulo/fundo/logo\n");
        if (fundo.id) UnloadTexture(fundo);
        if (titulo.id) UnloadTexture(titulo);
        if (logo.id) UnloadTexture(logo);
        UnloadTexture(reiTextura);
        CloseWindow();
        return 1;
    }
    // ---------------------------------------------------
    if (reiTextura.id == 0) {
        printf("Erro: falha ao criar textura do rei\n");
        CloseWindow();
        return 1;
    }

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
    const char *arquivoMapaTowerDefense = "assets/mapa/mapaTowerDefense";

    InitEnemies();
    InitPlayer();

    unsigned char map_map[GRID_HEIGHT][GRID_WIDTH] = {
        {0,3,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    };

    FILE *file = fopen("map.bin", "wb");
    if (!file) {
        perror("Erro ao criar arquivo binário");
        return 1;
    }

    for (int y = 0; y < GRID_HEIGHT; y++) {
        fwrite(map_map[y], sizeof(unsigned char), GRID_WIDTH, file);
    }

    unsigned char *mapTower = ReadMap(arquivoMapaTowerDefense);
    if (!mapTower) {
        printf("Erro: ReadMap retornou NULL\n");
        UnloadTexture(titulo);
        UnloadTexture(fundo);
        UnloadTexture(logo);
        UnloadTexture(reiTextura);
        CloseWindow();
        return 1;
    }

    float enemyTimer = 0;

    while (!WindowShouldClose()) {
        if (IsWindowResized()) {
            posicaoRei.x = (GetScreenWidth() - reiTextura.width) / 2;
            posicaoRei.y = (GetScreenHeight() - reiTextura.height) / 2;
            AtualizarCaminho(&pathStart, &pathEnd);
            ReposicionarInimigos(pathStart, pathEnd);
            RecenterTowers(GetScreenWidth(), GetScreenHeight());
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
            RecenterTowers(GetScreenWidth(), GetScreenHeight());
        }

        if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            jogoIniciado = true;

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
            DrawMap(mapTower);

            float dt = GetFrameTime();
            enemyTimer += dt;
            if (enemyTimer > 2.0f) {
                SpawnEnemy(pathStart);
                enemyTimer = 0;
            }

            UpdateEnemies(dt, pathStart, pathEnd);
            ReposicionarInimigos(pathStart, pathEnd);
            UpdatePlayer();

            ClearBackground((Color){20, 20, 30, 255});
            DrawEnemies();
            DrawTowers();
            DrawArchers();
            DrawText("Tower Defense - fase de inimigos", 10, 10, 20, WHITE);
        }

        EndDrawing();
    }

    // --- LIBERAÇÃO DE MEMÓRIA SEGURA ---
    free(map);
    UnloadTexture(titulo);
    UnloadTexture(fundo);
    UnloadTexture(logo);
    UnloadTexture(reiTextura);
    free(map);
    UnloadPlayer();
    CloseWindow();
    return 0;
}