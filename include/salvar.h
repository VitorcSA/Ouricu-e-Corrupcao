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

void UpdateSave(SaveData *save, float barComida, float barPoder, float barSaude, int level, int tempoPassado);
bool SaveNotEmpty(int slot);
void SaveGame(SaveData *save, int slot);
int LoadGame(SaveData *save, int slot);
int SelectSaveSlotMenu(const char *titulo, bool *isNovoJogo);
void DrawSaveMenuSprites(Texture2D archerTexture);
void StartNewGame(SaveData *save);
int saveSelection(SaveData *save, float *barcomida, float *barpoder, float *barsaude, float *level, int *tempoPassado);
void DeleteSave(int slot);

#endif