#include <raylib.h>
#include "HUD.h"

void desenharRetangulo(int alturaImagem, int posYSprite){
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    int posY = posYSprite - 5; 
    int alturaRetangulo = screenHeight - posY;          

    DrawRectangle(0, posY, screenWidth, alturaRetangulo, GRAY);

}

void desenharRei(Texture2D reiTextura, int posx, int posy){
    DrawTexture(reiTextura,
            posx,
            posy,
            WHITE);
}