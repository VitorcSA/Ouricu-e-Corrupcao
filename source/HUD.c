#include <raylib.h>
#include "HUD.h"

void desenharRetangulo(int alturaImagem){
    DrawRectangle(0, 
        (int)((GetScreenHeight() / 4) + alturaImagem/2 + 75), 
        GetScreenWidth(), 
        GetScreenWidth() - (int)((GetScreenHeight() / 4) + alturaImagem/2 + 30), 
        GRAY);
}

void desenharRei(Texture2D reiTextura, int largura, int altura){
    DrawTexture(reiTextura,
            (int)((GetScreenWidth() - largura) / 2 - 10),
            (int)((GetScreenHeight() / 4) + altura/2 + 60),
            WHITE);
}