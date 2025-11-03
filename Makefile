default:
	gcc -o main.exe source/main.c source/HUD.c -I include -L lib -lraylib -lgdi32 -lwinmm