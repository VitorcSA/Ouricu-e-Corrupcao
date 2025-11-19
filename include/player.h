#ifndef PLAYER_H
#define PLAYER_H

#include <raylib.h>

#define MAX_TOWERS 50

#define MAX_ARCHERS 50
#define MAX_WIZARDS 50
#define MAX_CANNONS 50

#define MAX_ARROWS 200
#define MAX_FIREBALLS 200
#define MAX_CANNONBALLS 200

#define ARCHER_QT_FRAMES_SHOOT 14
#define WIZARD_QT_FRAMES_SHOOT 8
#define CANNON_QT_FRAMES_SHOOT 5

#define ARROW_QT_FRAMES 1
#define FIREBALL_QT_FRAMES 12
#define CANNONBALL_QT_FRAMES 1

#define ARCHER_QT_FRAMES_IDLE 9
#define WIZARD_QT_FRAMES_IDLE 7
#define CANNON_QT_FRAMES_IDLE 1

#define ARROW_SPEED 400.0f
#define FIREBALL_SPEED 400.0f
#define CANNONBALL_SPEED 600.0f

#define ARCHER_SHOT_TIME 1.0f
#define WIZARD_SHOT_TIME 1.2f
#define CANNON_SHOT_TIME 1.0f

#define ARROW_DAMAGE 10
#define FIREBALL_DAMAGE 15
#define CANNONBALL_DAMAGE 20

#define GOLD_FOR_ENEMY 15
#define GOLD_FOR_ORC 50

typedef struct 
{
    Vector2 pos;
    Vector2 basePos;
    float size;
    float shotTimer;
    float frameTime;
    int frame;
    bool isShooting;
    bool active;
}Players;

typedef struct
{
    Vector2 pos;
    Vector2 target;
    float size;
    float speed;
    float frameTime;
    int frame;
    int enemyIndex;
    int orcIndex;
    bool active;
}Projects;

typedef struct {
    Vector2 pos;
    Vector2 basePos;
    float size;
    bool active;
    bool hasDefender;
} Tower;

extern int playerGold;
extern int towerCount;
extern int archerCount;
extern int wizardCount;
extern int cannonCount;
extern int ownedArchers;
extern int ownedWizards;
extern int ownedCannons;
extern int ownedTowers;
extern int towerPrice;

extern bool wizardUnlocked;
extern bool cannonUnlocked;

extern Tower tower[MAX_TOWERS];
extern Players archers[MAX_ARCHERS];
extern Players wizards[MAX_WIZARDS];
extern Players cannons[MAX_CANNONS];
extern Projects arrows[MAX_ARROWS];
extern Projects fireballs[MAX_FIREBALLS];
extern Projects cannonballs[MAX_CANNONBALLS];

extern Texture2D torreTexture;

extern Texture2D archerTexture;
extern Texture2D archerIdeleTexture;
extern Texture2D arrowTexture;

extern Texture2D wizardTexture;
extern Texture2D idlewizardTexture;
extern Texture2D fireballTexture;

extern Texture2D cannonTextureIdle;
extern Texture2D cannonTextureShot;
extern Texture2D cannonballTexture;

void InitPlayer(void);
void UpdatePlayer(unsigned char *mapa);

void DrawTowers(void);
void RecenterTowers(int newWidth, int newHeight);
void drawProjects(Projects *project, Texture2D projectTexture, bool hasFrames, int max, int quantFrames);
void DrawPlayer(Players *player, Texture2D playerIdleTexture, Texture2D playerShootingTexture, int playerCount, int quantFrameShot, int quantFrameIdle);
void DrawPlayerInvertido(Players *player, Texture2D playerIdleTexture, Texture2D playerShootingTexture, int playerCount, int quantFrameShot, int quantFrameIdle);

void AddPlayer(Players *player, Vector2 pos, int max, int *playerCount, int screenWidth, int screenHeight);
void shotProject(Projects *project, Vector2 start, Vector2 target, float speed, int enemyIndex, int max, bool hasFrames);

void BuyArcher();
void BuyWizard();
void BuyCannon();
void BuyTower();

void UnloadPlayer(void);

#endif