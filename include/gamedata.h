#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <raylib.h>

typedef enum State{
	KEEP = -1,
	LOGO = 0,
	TITLE,
	SAVE,
	TUTORIAL,
	MENU,
	SHOP,
	GAME
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
	Texture2D king;
}Assets;

typedef struct Player{
	bool unlockedCannon;
	bool unlockedWizard;
	int gold;
	int level;
	int timePassed;
	float health;
	float food;
	float power;
}Player;

typedef struct Save{
	const char *title;
	Vector2 mousePos;
	Player *player;
	Texture2D *sprites;
}Save;

typedef struct Button{
	Rectangle dimensions;
	Color lines;
	State whereToGo;
	Color color;
	Color hoverColor;
	char text[50];
	bool isRounded;
	bool isHover;
	int font;
}Button;

typedef struct Menu{
	Button buttonGame;
	Button buttonShop;
	const Player *player;
	Texture2D background;
	Texture2D *king;
}Menu;

typedef struct Data{
	State state;
	Logo logo;
	Title title;
	Save save;
	Menu menu;
	Player player;
	Assets assets;
}Data;

void initData(Data *data);

void loadAssets(Assets *assets);

#endif
