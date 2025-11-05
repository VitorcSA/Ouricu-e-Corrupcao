#ifndef MAPA_H
#define MAPA_H

#include "raylib.h"
#include <stdbool.h>

#define GRID_WIDTH 10
#define GRID_HEIGHT 8

typedef enum {
    CELL_PATH,
    CELL_BUILDABLE,
    CELL_OCCUPIED
} CellType;

extern CellType grid[GRID_HEIGHT][GRID_WIDTH];

// Funções públicas
void InitMap(void);
void DrawMap(void);
bool HandleTowerPlacement(void);

#endif
