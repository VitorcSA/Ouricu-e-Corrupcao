#ifndef TELAS_H
#define TELAS_H

#include "gamedata.h"
#include <raylib.h>
#include <math.h>

void TelaLogo(Texture2D logo);

void TelaTitulo(Texture2D titulo, Texture2D fundo);
void TelaGameOver(Texture2D fundo);

State updateLogoScreen(void* data);
void drawLogoScreen(void *data);

State updateTitleScreen(void *data);
void drawTitleScreen(void *data);

State updateTutorial(void *data);
void DrawTutorial(void *data);

State updateMenuScreen(void *data);
void drawMenuScreen(void *data);

#endif
