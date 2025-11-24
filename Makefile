ifeq ($(OS),Windows_NT)
	TARGET = main.exe
	LIBS = -lraylib -lgdi32 -lwinmm
else
	TARGET = main
	LIBS = -lraylib -lm -lpthread -ldl -lrt
endif

INCLUDE = -I include
LIBDIR = -L lib

default:
	gcc -o $(TARGET) source/*.c $(INCLUDE) $(LIBDIR) $(LIBS)