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

typedef struct 
{
    Vector2 pos;
    Vector2 basePos;
    int frame;
    float frameTime;
    float shotTimer;
    bool isShooting;
    bool active;
}Players;

typedef struct
{
    Vector2 pos;
    Vector2 target;
    bool active;
    float speed;
    int frame;
    float frameTime;
    int enemyIndex;
    int orcIndex;
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
void UpdatePlayer(void);
void DrawTowers(void);
void RecenterTowers(int newWidth, int newHeight);
void AddPlayer(Players *player, Vector2 pos, int max, int *playerCount, float screenWidth, float screenHeight);
void shotProject(Projects *project, Vector2 start, Vector2 target, float speed, int enemyIndex, int max, bool hasFrames);
void UnloadPlayer(void);
void InitArchers(void);
void AddArcher(Vector2 pos);
void UpdateArchers(float dt);
void drawProjects(Projects *project, Texture2D projectTexture, bool hasFrames, int max, int quantFrames);
void DrawPlayer(Players *player, Texture2D playerIdleTexture, Texture2D playerShootingTexture, int playerCount, int quantFrameShot, int quantFrameIdle);
void DrawArchers(void);
void UnloadArchers(void);
void DrawArrows(void);
void UnloadArrows(void);
void AddWizard(Vector2 towerPos, float towerSize);
void DrawWizards(void);
void InitFireballs(void);
void ShootFireball(Vector2 start, Vector2 target, int enemyIndex);
void UpdateFireballs(float dt);
void DrawFireballs(void);
void UnloadFireballs(void);
void InitCannons(void);
void AddCannon(Vector2 towerPos, float towerSize);
void UpdateCannons(float dt);
void DrawCannons(void);
void UnloadCannons(void);
void DrawCannonballs(void);

#endif