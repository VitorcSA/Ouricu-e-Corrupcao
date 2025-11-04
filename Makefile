default:
	gcc -o main.exe source/main.c source/HUD.c source/telaInicio.c source/enemies.c -I include -L lib -lraylib -lgdi32 -lwinmm