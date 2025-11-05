#ifndef MAPA_H
#define MAPA_H

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h>

// Definição das dimensões da grade
#define GRID_WIDTH 15   // Ajuste conforme necessário
#define GRID_HEIGHT 15  // Ajuste conforme necessário

// Definição de constantes para tipos de células (se usadas em HandleTowerPlacement)
#define CELL_BUILDABLE 0
#define CELL_OCCUPIED  1
#define CELL_PATH      2

// Protótipos das funções
unsigned char *ReadMap(const char *fileName);
Color CheckTile(unsigned char tile);
void DrawMap(unsigned char *self);
bool HandleTowerPlacement(void); // Descomente se a função for usada

#endif // MAPA_H