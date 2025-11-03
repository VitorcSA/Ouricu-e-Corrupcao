#include <raylib.h>
#include "HUD.h"

void desenharRetangulo(int alturaImagem){
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    int posY = (screenHeight / 2) + (alturaImagem / 2); 
    int alturaRetangulo = screenHeight - posY;          

    DrawRectangle(0, posY, screenWidth, alturaRetangulo, GRAY);

}

void desenharRei(Texture2D reiTextura, int posx, int posy){
    DrawTexture(reiTextura,
            posx,
            posy,
            WHITE);
}