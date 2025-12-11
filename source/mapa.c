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
static Texture2D buildable1;
static Texture2D buildable2;
static Texture2D parede;
static Texture2D portaInferiorDireita;
static Texture2D portaInferiorEsquerda;
static Texture2D portaSuperiorDireita;
static Texture2D portaSuperiorEsquerda;

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
    Image imgroundEnemyDI = LoadImage("assets/mapa/groundEnemyDireitaInferior.png");
    groundEnemyDireitaInferior = LoadTextureFromImage(imgroundEnemyDI);
    UnloadImage(imgroundEnemyDI);

    Image imgroundEnemyDS = LoadImage("assets/mapa/groundEnemyDireitaSuperior.png");
    groundEnemyDireitaSuperior = LoadTextureFromImage(imgroundEnemyDS);
    UnloadImage(imgroundEnemyDS);

    Image imgroundEnemyEI = LoadImage("assets/mapa/groundEnemyEsquerdaInferior.png");
    groundEnemyEsquerdaInferior = LoadTextureFromImage(imgroundEnemyEI);
    UnloadImage(imgroundEnemyEI);

    Image imgroundEnemyES = LoadImage("assets/mapa/groundEnemyEsquerdaSuperior.png");
    groundEnemyEsquerdaSuperior = LoadTextureFromImage(imgroundEnemyES);
    UnloadImage(imgroundEnemyES);

    Image imgroundEnemyH = LoadImage("assets/mapa/groundEnemyHorizontal.png");
    groundEnemyHorizontal = LoadTextureFromImage(imgroundEnemyH);
    UnloadImage(imgroundEnemyH);

    Image imgroundEnemyV = LoadImage("assets/mapa/groundEnemyVertical.png");
    groundEnemyVertical = LoadTextureFromImage(imgroundEnemyV);
    UnloadImage(imgroundEnemyV);

    Image imBuildable1 = LoadImage("assets/mapa/buildable1.png");
    buildable1 = LoadTextureFromImage(imBuildable1);
    UnloadImage(imBuildable1);

    Image imBuildable2 = LoadImage("assets/mapa/buildable2.png");
    buildable2 = LoadTextureFromImage(imBuildable2);
    UnloadImage(imBuildable2);

    Image imParede = LoadImage("assets/mapa/parede.png");
    parede = LoadTextureFromImage(imParede);
    UnloadImage(imParede);

    Image imPortaSuperiorDireita = LoadImage("assets/mapa/portaSuperiorDireita.png");
    portaSuperiorDireita = LoadTextureFromImage(imPortaSuperiorDireita);
    UnloadImage(imPortaSuperiorDireita);

    Image importaSuperiorEsquerda = LoadImage("assets/mapa/portaSuperiorEsquerda.png");
    portaSuperiorEsquerda = LoadTextureFromImage(importaSuperiorEsquerda);
    UnloadImage(importaSuperiorEsquerda);

    Image importaInferiorDireita = LoadImage("assets/mapa/portaInferiorDireita.png");
    portaInferiorDireita = LoadTextureFromImage(importaInferiorDireita);
    UnloadImage(importaInferiorDireita);

    Image importaInferiorEsquerda = LoadImage("assets/mapa/portaInferiorEsquerda.png");
    portaInferiorEsquerda = LoadTextureFromImage(importaInferiorEsquerda);
    UnloadImage(importaInferiorEsquerda);
}

Texture2D CheckTile(unsigned char tile) {
    switch (tile) {
        case 0: return buildable1;
        case 1: return groundEnemyHorizontal;
        case 2: return groundEnemyDireitaInferior;
        case 3: return groundEnemyVertical;
        case 4: return groundEnemyEsquerdaSuperior;
        case 5: return groundEnemyDireitaSuperior;
        case 6: return groundEnemyEsquerdaInferior;
        case 7: return parede;
        case 8: return portaInferiorEsquerda;
        case 9: return portaSuperiorEsquerda;
        case 10: return portaInferiorDireita;
        case 11: return portaSuperiorDireita;
        case 12: return buildable2;
        default: return buildable1;
    }
}

void DrawMap(unsigned char *self) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    float cellWidth  = (float)screenWidth  / GRID_WIDTH;
    float cellHeight = (float)screenHeight / GRID_HEIGHT;

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {

            unsigned char tile = self[y * GRID_WIDTH + x];
            Texture2D texture;

            // grama com 2 tipos
            if (tile == 0) {
                int choice = (x + y) % 2;
                texture = (choice == 0) ? buildable1 : buildable2;
            } else {
                texture = CheckTile(tile);
            }

            Rectangle dest   = (Rectangle){ x * cellWidth, y * cellHeight, cellWidth, cellHeight };
            Rectangle source = (Rectangle){ 0, 0, (float)texture.width, (float)texture.height };
            Vector2   origin = (Vector2){ 0, 0 };

            DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
        }
    }
}


void drawLinesMap(unsigned char *mapa){
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Calcular tamanho das células para preencher a tela
    float cellWidth = (float)screenWidth / GRID_WIDTH;
    float cellHeight = (float)screenHeight / GRID_HEIGHT;

    Vector2 mouse = GetMousePosition();

    int gridX = (int)(mouse.x / cellWidth);
    int gridY = (int)(mouse.y / cellHeight);
    Rectangle dest = { gridX * cellWidth, gridY * cellHeight, cellWidth, cellHeight };

    if(mapa[gridY * GRID_WIDTH + gridX] != 0)DrawRectangleRec(dest, Fade(RED, 0.3f));
    else DrawRectangleRec(dest, Fade(WHITE, 0.3f));
}
