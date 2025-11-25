#ifndef SALVAR_H
#define sALVAR_H

typedef struct {
    int levelAtual;
    int gold;
    int tempoPassado;
    
    bool wizardUnlocked;
    bool cannonUnlocked;

    float barsaude;
    float barcomida;
    float barpoder;

} SaveData;

bool SaveNotEmpty(int slot);
void SaveGame(SaveData *save, int slot);
int LoadGame(SaveData *save, int slot);
int SelectSaveSlotMenu(const char *titulo);
void StartNewGame(SaveData *save);
void SaveGold(int playerGold);
int LoadGold(void);

#endif