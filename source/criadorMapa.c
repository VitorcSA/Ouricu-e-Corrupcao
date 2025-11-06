#include "criadorMapa.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int criadorDeMapa(const char *nomeDoArquivo, int altura, int largura) {
    // Validações
    if (altura != 15 || largura != 15) {
        fprintf(stderr, "Erro: mapa deve ser 15x15 (recebido: %dx%d)\n", altura, largura);
        return 1;
    }

    // Mapa fixo 15x15
    static const unsigned char mapa_inicial[15][15] = {
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,1,1,1,1,0,0,0,0,0,0,0,0},
        {0,0,0,1,0,0,1,0,0,0,0,0,0,0,0},
        {0,0,0,1,0,0,1,0,0,0,0,0,0,0,0},
        {0,0,0,1,0,0,1,0,0,0,1,1,1,1,1},
        {1,1,1,1,0,0,1,0,0,0,1,0,0,0,0},
        {0,0,0,0,0,0,1,0,0,0,1,0,0,0,0},
        {0,0,0,0,0,0,1,0,1,1,1,0,0,0,0},
        {0,0,0,0,0,0,1,1,1,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    };

    FILE *file = fopen(nomeDoArquivo, "wb");
    if (!file) {
        perror("Erro ao criar arquivo binário");
        return 1;
    }

    for (int y = 0; y < altura; y++) {
        fwrite(mapa_inicial[y], sizeof(unsigned char), largura, file);
    }

    fclose(file);
    printf("Arquivo %s criado com sucesso!\n", nomeDoArquivo);
    return 0;
}

bool verificarSeMapaExiste(const char *nomeDoArquivo) {
    FILE *file = fopen(nomeDoArquivo, "rb");
    if(file){
        return true;
    }
    perror("Erro No arquivo:");
    return false;
}