#include "gamedata.h"
#include "raylib.h"

void initData(Data *data){
	data->logo.texture.id = -1;
	data->title.background.id = -1;
	data->menu.background.id = -1;
	data->menu.king = &data->assets.king;
	data->menu.player = &data->player;
	data->save.player = &data->player;
	data->save.sprites = &data->assets.archer;

}

void loadAssets(Assets *assets){

	Image torre = LoadImage("assets/torre_colocar.png");
	ImageResize(&torre, 64, 64);
	assets->tower = LoadTextureFromImage(torre);
	UnloadImage(torre);
	
	Image image = LoadImage("assets/rei.png");
	ImageResize(&image, 200,200);
	assets->king = LoadTextureFromImage(image);
	UnloadImage(image);

	assets->archer = LoadTexture("assets/InimigosAnimation/Shot.png");

	assets->idleArcher = LoadTexture("assets/InimigosAnimation/Idle.png");

	assets->arrow = LoadTexture("assets/InimigosAnimation/Arrow.png");

	assets->wizard = LoadTexture("assets/InimigosAnimation/ShotWizard.png");

	assets->idleWizard = LoadTexture("assets/InimigosAnimation/IdleWizard.png");

	assets->fireball = LoadTexture("assets/InimigosAnimation/Fireball.png");

	assets->idleCannon = LoadTexture("assets/InimigosAnimation/idlecannon.png");

	assets->shotCannon = LoadTexture("assets/InimigosAnimation/shotcannon.png");

	assets->cannonball = LoadTexture("assets/InimigosAnimation/cannonball.png");

}
