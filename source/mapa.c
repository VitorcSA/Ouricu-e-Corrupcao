#include "mapa.h"
#include <stdio.h>   

// apenas define o grid real (não o enum!)
CellType grid[GRID_HEIGHT][GRID_WIDTH];

void InitMap(void) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid[y][x] = CELL_BUILDABLE;
        }
    }

    // Caminho horizontal no meio
    for (int x = 0; x < GRID_WIDTH; x++) {
        grid[GRID_HEIGHT / 2][x] = CELL_PATH;
    }
}

void DrawMap(void) {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    float cellWidth = (float)screenWidth / GRID_WIDTH;
    float cellHeight = (float)screenHeight / GRID_HEIGHT;

    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            Rectangle cell = { x * cellWidth, y * cellHeight, cellWidth, cellHeight };

            Color color;
            switch (grid[y][x]) {
                case CELL_PATH: color = BROWN; break;
                case CELL_BUILDABLE: color = GREEN; break;
                case CELL_OCCUPIED: color = DARKGREEN; break;
            }

            DrawRectangleRec(cell, color);
            DrawRectangleLines(cell.x, cell.y, cell.width, cell.height, BLACK);
        }
    }
}

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
            if (grid[gridY][gridX] == CELL_BUILDABLE) {
                grid[gridY][gridX] = CELL_OCCUPIED;
                return true;
            } else if (grid[gridY][gridX] == CELL_PATH) {
                printf("⚠️ Não pode colocar torre no caminho!\n");
            } else if (grid[gridY][gridX] == CELL_OCCUPIED) {
                printf("Já tem uma torre aqui!\n");
            }
        }
    }
    return false;
}

