#ifndef DIALOGO_H
#define DIALOGO_H

#include <stdbool.h>


typedef struct {
    EffectType type;
    float value;      // quanto altera (ex: +20% ou -10%)
    int duration;     // em quantas ondas dura
    bool active;
} ActiveEffect;

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

typedef enum {
    DIALOG_SIMPLE,
    DIALOG_CHOICES
} DialogType;

typedef struct {
    int id;
    char text[256];
    char option1[128];
    char option2[128];
    DialogType type;
    bool active;
} Dialog;

#define MAX_ACTIVE_EFFECTS 20
ActiveEffect efeitosAtivos[MAX_ACTIVE_EFFECTS];


void DrawDialogBox(const char *text, int screenWidth, int screenHeight);

#endif