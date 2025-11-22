#include <stdio.h>
#include <player.h>
#include <raylib.h>

void SaveGold(int playerGold)
{
    FILE *f = fopen("save_gold.bin", "wb");
    if (f == NULL) return;

    fwrite(&playerGold, sizeof(int), 1, f);
    fclose(f);
}

int LoadGold()
{
    FILE *f = fopen("save_gold.bin", "rb");
    if (f == NULL)
        return 0;

    int playerGold = 0;
    fread(&playerGold, sizeof(int), 1, f);
    fclose(f);

    return playerGold;
}