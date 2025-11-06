#ifndef CRIADORMAPA_H
#define CRIADORMAPA_H

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int criadorDeMapa(const char *nomeDoArquivo, int altura, int largura);

bool verificarSeMapaExiste(const char *nomeDoArquivo);

#endif // CRIADORMAPA_H