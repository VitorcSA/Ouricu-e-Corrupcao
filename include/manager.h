#ifndef MANAGER_H
#define MANAGER_H

#include "gamedata.h"
#include <stdbool.h>

typedef State (*Update)(void *data);
typedef void (*Draw)(void *data);

typedef struct Manager{
	Update update;
	Draw draw;
	void *funcData;
	Data data;
}Manager;

bool initGame(void);

bool updateGame(void);

bool drawGame(void);

void endGame(void);

void setUpdateFunc(Update update);

void setDrawUpdate(Draw draw);

#endif
