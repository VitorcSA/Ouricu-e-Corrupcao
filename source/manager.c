#include "manager.h"
#include "gamedata.h"
#include "raylib.h"
#include "salvar.h"
#include "telas.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define GAME_NAME "Poder e corrupção"

void initManager(Manager *manager);
void setUpdateFunc(Update update);
void setDrawUpdate(Draw draw);
bool switchState(State state);
void configWindow(void);

Manager manager;

void initManager(Manager *manager){
	manager->update = updateLogoScreen;
	manager->draw = drawLogoScreen;
	manager->funcData = &manager->data.logo;
	manager->data.state = LOGO;

	initData(&manager->data);
}

bool initGame(void){
	configWindow();
	initManager(&manager);
	loadAssets(&manager.data.assets);

	return true;
}

bool updateGame(void){
	if(WindowShouldClose()) return false;

	if(IsKeyPressed(KEY_F11)) ToggleFullscreen();


	if(manager.update){
		State state = manager.update(manager.funcData);
		if(state != KEEP) switchState(state);
	};

	return true;
}

bool drawGame(void){
	BeginDrawing();
		ClearBackground(BLACK);
		if(manager.draw) manager.draw(manager.funcData);
	EndDrawing();
	return true;
};

void endGame(void){
	CloseAudioDevice();
}

void configWindow(void){
	//Configuração da janela
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	InitWindow(1280, 720,GAME_NAME);
	SetExitKey(KEY_NULL);
	SetTargetFPS(60);
	InitAudioDevice(); 

};

void setUpdateFunc(Update update){
	manager.update = update;
}

void setDrawUpdate(Draw draw){
	manager.draw = draw;

}

bool switchState(State state){
	switch (state) {
		case KEEP:
			break;

		case LOGO:
			manager.draw = drawLogoScreen;
			manager.update = updateLogoScreen;
			manager.funcData = &manager.data.logo;
			manager.data.state = state;

			break;
		case TITLE:
			manager.draw = drawTitleScreen;
			manager.update = updateTitleScreen;
			manager.funcData = &manager.data.title;
			manager.data.state = state;
			
			break;
		case SAVE:
			manager.draw = drawSaveMenu;
			manager.update = updateSaveMenu;
			manager.funcData = &manager.data.save;
			manager.data.state = state;

			break;
		case TUTORIAL:
			manager.draw = DrawTutorial;
			manager.update = updateTutorial;
			manager.funcData = NULL;
			manager.data.state = state;

			break;
		case MENU:
			manager.draw = drawMenuScreen;
			manager.update = updateMenuScreen;
			manager.funcData = &manager.data.menu;
			manager.data.state = state;

			break;
	}

	return true;
};


