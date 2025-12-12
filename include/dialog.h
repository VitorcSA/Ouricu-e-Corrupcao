#ifndef DIALOGO_H
#define DIALOGO_H

#include <stdbool.h>


typedef enum {
    EFFECT_NONE = 0,
    EFFECT_TOWER_DAMAGE_UP,
    EFFECT_TOWER_DAMAGE_DOWN,
    EFFECT_TOWER_FIRE_RATE_UP,
    EFFECT_TOWER_FIRE_RATE_DOWN,
    EFFECT_GOLD_BONUS,
    EFFECT_GOLD_PENALTY,
    EFFECT_ENEMY_SPEED_UP,
    EFFECT_ENEMY_SPEED_DOWN,
    EFFECT_HEAL_PLAYER,
    EFFECT_HURT_PLAYER
} EffectType;

typedef struct {
    EffectType type;
    float value;      // quanto altera (ex: +20% ou -10%)
    int duration;     // em quantas ondas dura
    bool active;
} ActiveEffect;

typedef struct {
    int numOpcoes;
    int id;
    char text[256];
    char option1[128];
    char option2[128];
    bool active;
} Dialog;

#define NUM_MAX_PERGUNTAS 3

Dialog *carregarPergunta(const char *arquivo, int numAleatorio);
void DrawDialogScreen(Dialog *dialogo, int screenWidth, int screenHeight);
int CheckDialogClick(Dialog *dialogo, int screenWidth, int screenHeight);

#endif