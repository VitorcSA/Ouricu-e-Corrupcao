default:
	gcc -o main.exe source/*.c -I include -L lib -lraylib -lgdi32 -lwinmm