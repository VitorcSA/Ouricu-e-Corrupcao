#include "mapa.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static Texture2D groundEnemyVertical;
static Texture2D groundEnemyHorizontal;
static Texture2D groundEnemyDireitaSuperior;
static Texture2D groundEnemyDireitaInferior;
static Texture2D groundEnemyEsquerdaSuperior;
static Texture2D groundEnemyEsquerdaInferior;
static Texture2D buildable;


bool CheckFile(FILE *file){
    if(file){
        return true;
    }
    perror("Erro No arquivo:");
    return false;
}

unsigned char *ReadMap(const char *fileName) {

    unsigned char *map = malloc(sizeof(unsigned char) * GRID_HEIGHT * GRID_WIDTH);

    FILE *file = fopen(fileName, "rb");

    if (CheckFile(file))
    {
        fread(map, sizeof(unsigned char), GRID_HEIGHT * GRID_WIDTH, file);
        fclose(file);

        return map;
    }

    fclose(file);
    free(map);

    return NULL;
}

void initTiles(){
    Image imgroundEnemyDI = LoadImage("assets/torre_colocar.png");
    groundEnemyDireitaInferior = LoadTextureFromImage(imgroundEnemyDI);
    UnloadImage(imgroundEnemyDI);

    Image imgroundEnemyDS = LoadImage("assets/torre_colocar.png");
    groundEnemyDireitaSuperior = LoadTextureFromImage(imgroundEnemyDS);
    UnloadImage(imgroundEnemyDS);

    Image imgroundEnemyEI = LoadImage("assets/torre_colocar.png");
    groundEnemyEsquerdaInferior = LoadTextureFromImage(imgroundEnemyEI);
    UnloadImage(imgroundEnemyEI);

    Image imgroundEnemyES = LoadImage("assets/torre_colocar.png");
    groundEnemyEsquerdaSuperior = LoadTextureFromImage(imgroundEnemyES);
    UnloadImage(imgroundEnemyES);

    Image imgroundEnemyH = LoadImage("assets/torre_colocar.png");
    groundEnemyHorizontal = LoadTextureFromImage(imgroundEnemyH);
    UnloadImage(imgroundEnemyH);

    Image imgroundEnemyV = LoadImage("assets/torre_colocar.png");
    groundEnemyVertical = LoadTextureFromImage(imgroundEnemyV);
    UnloadImage(imgroundEnemyV);

}

Color CheckTile(unsigned char tile) {
    switch (tile) {
        case 0: return GREEN;
        case 1: return GRAY;
        case 2: return DARKGREEN;
        case 3: return BLUE;
        default: return BLACK;
    }
}

void DrawMap(unsigned char *self) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Calcular tamanho das células para preencher a tela
    float cellWidth = (float)screenWidth / GRID_WIDTH;
    float cellHeight = (float)screenHeight / GRID_HEIGHT;

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            Rectangle cell = { x * cellWidth, y * cellHeight, cellWidth, cellHeight };

            unsigned char tile = self[y * GRID_WIDTH + x];
            Color color = CheckTile(tile);

            DrawRectangleRec(cell, color);
            DrawRectangleLinesEx(cell, 1.0f, BLACK); // Bordas finas
        }
    }
}
/*
// tipo de retorno precisa bater com o .h -> bool
bool HandleTowerPlacement(void) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse = GetMousePosition();

        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
        float cellWidth = (float)screenWidth / GRID_WIDTH;
        float cellHeight = (float)screenHeight / GRID_HEIGHT;

        int gridX = mouse.x / cellWidth;
        int gridY = mouse.y / cellHeight;

        if (gridX >= 0 && gridX < GRID_WIDTH && gridY >= 0 && gridY < GRID_HEIGHT) {
            if (grid[gridY * GRID_WIDTH + gridX] == CELL_BUILDABLE) {
                grid[gridY * GRID_WIDTH + gridX] = CELL_OCCUPIED;
                return true;
            } else if (grid[gridY * GRID_WIDTH + gridX] == CELL_PATH) {
                printf("⚠️ Não pode colocar torre no caminho!\n");
                } else if (grid[gridY * GRID_WIDTH + gridX] == CELL_OCCUPIED) {
                    printf("Já tem uma torre aqui!\n");
                }
        }
    }
    return false;
}
*/
