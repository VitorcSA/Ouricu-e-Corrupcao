#ifndef SALVAR_H
#define SALVAR_H

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

void UpdateSave(SaveData *save, float barComida, float barPoder, float barSaude, int level, int tempoPassado);
bool SaveNotEmpty(int slot);
void SaveGame(SaveData *save, int slot);
int LoadGame(SaveData *save, int slot);
void StartNewGame(SaveData *save);
void DeleteSave(int slot);

State updateSaveMenu(void *data);
void drawSaveMenu(void *data);

#endif
