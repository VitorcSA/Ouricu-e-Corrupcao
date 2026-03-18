#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <raylib.h>

typedef enum State{
	KEEP = -1,
	LOGO = 0,
	TITLE,
	SAVE,
	MENU
}State;

typedef enum Fase{
    APPEAR,
    WAIT,
    DISAPPEAR,
}Fase;

typedef struct Logo{

	Texture2D texture;
	float alpha;
	Fase fase;

}Logo;

typedef struct TitleText{
	Texture string;
	Vector2 position;
}TitleText;

typedef struct Title{
	TitleText titleText;
	Texture2D background;
	bool transition;
	float alpha;
}Title;

typedef struct Assets{
	Texture2D tower;
	Texture2D archer;
	Texture2D idleArcher;
	Texture2D arrow;
	Texture2D wizard;
	Texture2D idleWizard;
	Texture2D fireball;
	Texture2D idleCannon;
	Texture2D shotCannon;
	Texture2D cannonball;
}Assets;

typedef struct Data{
	State state;
	Logo logo;
	Title title;
	Assets assets;
}Data;

void initData(Data *data);

void loadAssets(Assets *assets);

#endif
