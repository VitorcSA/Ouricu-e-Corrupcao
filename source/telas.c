#include <raylib.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "telas.h"
#include "gamedata.h"

#define DEFAULT_LOGO_PATH "assets/logo.png"
#define DEFAULT_TITLE_BACKGROUND_PATH "assets/fundotitulo.png"
#define DEFAULT_TITLE_STRING_PATH "assets/titulo.png"
#define FADE_SPEED 100.0f

bool updateLogo(Fase *fase,float *alpha);
bool updateTitle(void *data);
void initLogo(Logo *logo);
void initTitle(Title *title);

State updateLogoScreen(void* data){
	Logo *logo = (Logo *)data;

	if(logo->texture.id == -1) initLogo(logo);
	if(updateLogo(&logo->fase,&logo->alpha)) return TITLE;

	return KEEP;
}

void initLogo(Logo *logo){
	logo->alpha = 0.0f;
	logo->fase = APPEAR;

	Image logoImage = LoadImage(DEFAULT_LOGO_PATH);
	SetWindowIcon(logoImage);

	logo->texture = LoadTextureFromImage(logoImage);
	UnloadImage(logoImage);

};

void drawLogoScreen(void *data){
	const Logo *logo = (Logo *)data;
	DrawTexture(logo->texture,
                    (GetScreenWidth() - logo->texture.width) / 2,
                    (GetScreenHeight() - logo->texture.height) / 2,
                    (Color){255, 255, 255, (unsigned char)logo->alpha});
};

bool updateLogo(Fase *fase,float *alpha){
    float deltaTime = GetFrameTime();
    static float timer = 0.0f;

    switch (*fase)
    {
        case APPEAR:
            *alpha += (FADE_SPEED * deltaTime);
            if (*alpha >= 255.0f){
                *alpha = 255.0f;
                *fase = WAIT;
            }

            break;

        case WAIT:
            timer += deltaTime;
            if (timer >= 1.0f){
                timer = 0.0f;
                *fase =  DISAPPEAR; 
            }

            break;

        case DISAPPEAR:
            *alpha -= (FADE_SPEED * deltaTime);
            if (*alpha <= 0.0f){
                    *alpha = 0.0f;
                    return true;
            }
            break;
    }
    return false;
};

bool recenterTitle(Texture2D *title,Vector2 *titlePosition){
	
	if (IsWindowResized()) {
		titlePosition->x = (GetScreenWidth() - title->width) / 2.0f;
		titlePosition->y = (GetScreenHeight() - title->height) / 2.0f;
		
		return true;
	}
	
	return false;
};


State updateTitleScreen(void *data){
	if(updateTitle(data)) return SAVE;

	return KEEP;
};

bool updateTitle(void *data) {
	Title *title = (Title *)data;
	if(title->background.id == -1) initTitle(title);

	const uint8_t moveSpeed = 40;

	float deltaTime = GetFrameTime();

	recenterTitle(&title->titleText.string,&title->titleText.position);

	if(IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) title->transition = true;

        if (title->transition) {
            title->titleText.position.y -= moveSpeed * deltaTime;
            if (title->alpha > 0.0f) {
                title->alpha -= (FADE_SPEED * deltaTime);

		if (title->alpha <= 0.0f) {
			return true;
		}

            }
        }


    return false;
}

void initTitle(Title *title){
	title->alpha = 255.0f;
	title->transition = false;

	title->background = LoadTexture(DEFAULT_TITLE_BACKGROUND_PATH);
	title->titleText.string = LoadTexture(DEFAULT_TITLE_STRING_PATH);


	title->titleText.position = (Vector2){ (GetScreenWidth() - title->titleText.string.width) / 2.0f,
                          (GetScreenHeight() - title->titleText.string.height) / 2.0f };
}

void drawTitleScreen(void *data){
	Title *title = (Title *)data;

        ClearBackground(RAYWHITE);

        DrawTexturePro(title->background,
                       (Rectangle){0, 0,title->background.width,title->background.height},
                       (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
                       (Vector2){0, 0}, 
                       0.0f, 
                       WHITE);

        DrawTexture(title->titleText.string, (int)title->titleText.position.x, (int)title->titleText.position.y,
                    (Color){255, 255, 255, (unsigned char)title->alpha});

        if (!title->transition) {
            float brilho = (sinf(GetTime() * 3.0f) + 1.0f) / 2.0f;
            unsigned char intensidade = (unsigned char)(brilho * 255);
            const char *texto = "Pressione ENTER ou clique para continuar";
            DrawText(texto,
                     (GetScreenWidth() - MeasureText(texto, 20)) / 2,
                     GetScreenHeight() - 60, 20,
                     (Color){intensidade, intensidade, intensidade, 255});
            DrawText("[F11] alterna fullscreen", 10, 10, 20, GRAY);
        }
};

bool recenterText(const char *text,int fontSize,Vector2 *position){
	static bool borderless = false;

	if (IsKeyPressed(KEY_F11)) {
		borderless = !borderless;

		if (borderless) {
			ToggleFullscreen();

			position->x = MeasureText(text, fontSize) / 2.0f;
			position->y = GetScreenHeight() / 3.0f;
			return true;
			};
		ToggleFullscreen();

		SetWindowPosition(
			(GetMonitorWidth(0) - 1280) / 2,
			(GetMonitorHeight(0) - 720) / 2);
			
		position->x = MeasureText(text, fontSize) / 2.0f;
		position->y =  GetScreenHeight() / 3.0f;

		return true;
	}
	
	if (IsWindowResized()) {
		position->x =  MeasureText(text, fontSize) / 2.0f;
		position->y = GetScreenHeight() / 3.0f;
		
		return true;
	}
	
	return false;
};



void TelaGameOver(Texture2D fundo) {

    const char *textoGO = "GAME OVER";

    float alpha = 255.0f;
    float fadeSpeed = 120.0f;
    float moveSpeed = 40.0f;
    bool transition = false;

    int fontSize = 90;
    Vector2 posTexto = { 
        (GetScreenWidth() - MeasureText(textoGO, fontSize)) / 2.0f,
        GetScreenHeight() / 3.0f
    };

	while (!WindowShouldClose()) {

		float deltaTime = GetFrameTime();

		recenterText(textoGO,fontSize,&posTexto);

		// ENTER inicia fade + subida
		if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) transition = true;

        if (transition) {
            posTexto.y -= moveSpeed * deltaTime;
            if (alpha > 0.0f) {
                alpha -= fadeSpeed * deltaTime;
                if (alpha <= 0.0f) {
                    alpha = 0.0f;
			return;
		}
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        // Fundo
        DrawTexturePro(fundo,
                       (Rectangle){0, 0, fundo.width, fundo.height},
                       (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
                       (Vector2){0, 0},
                       0.0f,
                       WHITE);

        // Sombra (Glow)
        DrawText(textoGO,
                 posTexto.x + 4, posTexto.y + 4,
                 fontSize,
                 Fade((Color){40, 0, 0, 255}, alpha / 255.0f));

        // GAME OVER principal (pulsando)
        float pulso = 1.0f + 0.04f * sinf(GetTime() * 3.0f);
        int tamanhoFinal = fontSize * pulso;

        DrawText(textoGO,
                 (GetScreenWidth() - MeasureText(textoGO, tamanhoFinal)) / 2,
                 posTexto.y,
                 tamanhoFinal,
                 Fade(RED, alpha / 255.0f));

        // Mensagem piscando
        if (!transition) {
            float brilho = (sinf(GetTime() * 4.0f) + 1.0f) / 2.0f;
            unsigned char luz = brilho * 255;

            const char *msg = "Pressione ENTER para voltar para os saves";
            DrawText(msg,
                     (GetScreenWidth() - MeasureText(msg, 22)) / 2,
                     GetScreenHeight() - 80,
                     22,
                     (Color){luz, luz, luz, 255});
            
        }
        EndDrawing();
    }
}


