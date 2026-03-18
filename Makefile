ifeq ($(OS),Windows_NT)
	TARGET = main.exe
	LIBS = -lraylib -lgdi32 -lwinmm
	CLEAN = cls
else
	TARGET = main
	LIBS = -lraylib -lm -lpthread -ldl -lrt
	CLEAN = clear
endif

INCLUDE = -I include
LIBDIR = -L lib

compile:
	@gcc -o $(TARGET) source/*.c $(INCLUDE) $(LIBDIR) $(LIBS)

run:
	@$(CLEAN)
	@./$(TARGET)

test: compile run
	@echo iniciando teste
