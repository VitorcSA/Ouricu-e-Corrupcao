#ifndef DIALOGO_H
#define DIALOGO_H

#include <stdbool.h>

typedef enum {
    EFFECT_NONE = 0,

    EFFECT_TOWER_PRICE_BONUS,
    EFFECT_TOWER_PRICE_PENALTY,

    EFFECT_ARCHER_BONUS_DAMAGE,
    EFFECT_WIZARD_BONUS_DAMAGE,
    EFFECT_CANNON_BONUS_DAMAGE,

    EFFECT_ARCHER_PENALTY_DAMAGE,
    EFFECT_WIZARD_PENALTY_DAMAGE,
    EFFECT_CANNON_PENALTY_DAMAGE,

    EFFECT_PODER_BONUS,
    EFFECT_COMIDA_BONUS,
    EFFECT_SAUDE_BONUS,

    EFFECT_PODER_PENALTY,
    EFFECT_COMIDA_PENALTY,
    EFFECT_SAUDE_PENALTY,

    EFFECT_GOLD_BONUS_ENEMY,
    EFFECT_GOLD_BONUS_ORC,

    EFFECT_GOLD_PENALTY_ENEMY,
    EFFECT_GOLD_PENALTY_ORC,

    EFFECT_GOLD_BONUS,
    EFFECT_GOLD_PENALTY,
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

    EffectType effect1;
    float effectValue1;
    int effectDuration1;

    EffectType effect2;
    float effectValue2;
    int effectDuration2;

} Dialog;

#define MAX_NUM_PERGUNTAS 3
#define MAX_EFFECT_ACTIVE 64

extern ActiveEffect activeEffects[MAX_EFFECT_ACTIVE];
extern int activeCount;

Dialog *carregarPergunta(const char *arquivo, int numAleatorio);

void ApplyDialogEffect(Dialog *d, int option, int *gold, float *vida, float *comida, float *poder);
void UpdateEffects();
void CleanupEffects();
void resetEffects();

float GetTotalModifier(EffectType typeWanted);

int DrawDialogScreen(Dialog *dialogo, int screenWidth, int screenHeight);

#endif