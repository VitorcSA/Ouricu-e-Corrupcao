#include <raylib.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "HUD.h"
#include "telas.h"
#include "gamedata.h"

#define DEFAULT_LOGO_PATH "assets/logo.png"
#define DEFAULT_TITLE_BACKGROUND_PATH "assets/fundotitulo.png"
#define DEFAULT_TITLE_STRING_PATH "assets/titulo.png"
#define FADE_SPEED 100.0f

#define BTN_START_COR_HOVER                 (Color){0, 100, 0, 255}
#define BTN_START_COR                       DARKGREEN
#define BTN_START_LINES_COR                 GOLD
#define BTN_START_WIDTH                     220
#define BTN_START_HEIGHT                    60
#define BTN_START_X                         (screenWidth - BTN_START_WIDTH) / 2
#define BTN_START_Y                         fundoHeight - 120
#define BTN_START_FONTE                     30

#define BTN_LOJA_COR_HOVER                  (Color){20, 20, 30, 180}
#define BTN_LOJA_COR                        (Color){40, 40, 50, 200}
#define BTN_LOJA_LINES_COR                  (Color){255, 255, 255, 120}
#define BTN_LOJA_WIDTH                      180
#define BTN_LOJA_HEIGHT                     60
#define BTN_LOJA_X                          screenWidth - BTN_LOJA_WIDTH - 20
#define BTN_LOJA_Y                          20
#define BTN_LOJA_FONTE                      22

#define TUTORIAL_DIALOGS \
	"Você é o rei e tem que defender seu reino de inimigos.",\
	"Para isso você usará torres e defensores.",\
	"Clique com o botão esquerdo em algum lugar no campo para adicionar uma torre.",\
	"Clique com o botão direito na torre para abrir o menu de defensores.",\
	"Escolha um defensor e posicione-o sobre a torre.",\
	"Com o tempo você vai desbloqueando novos defensores.",\
	"Cada defensor tem distâncias de ataque e danos diferentes.",\
	"Lembre-se: ao gastar muitos recursos apenas em defesa, esquecendo de seu povo:",\
	"GAME OVER"

bool updateLogo(Fase *fase,float *alpha);
bool updateTitle(void *data);
void initLogo(Logo *logo);
void initTitle(Title *title);
void initMenu(Menu *menu);
void drawButton(const Button *button);
State updateButton(Button *button);
void createButton(Button *button,State whereToGo,Color color,Color colorHover,Color lines,bool isRounded,const char *text,Rectangle dimensions,int font);

void drawLogoScreen(void *data){
	const Logo *logo = (Logo *)data;
	DrawTexture(logo->texture,
                    (GetScreenWidth() - logo->texture.width) / 2,
                    (GetScreenHeight() - logo->texture.height) / 2,
                    (Color){255, 255, 255, (unsigned char)logo->alpha});
};

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

State updateTutorial(void *data){
            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return MENU;

	    return KEEP;
};

void DrawTutorial(void *data) {
    const int fontSize = 24;
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    int y = screenHeight / 6;

    DrawText("TUTORIAL", (screenWidth - MeasureText("TUTORIAL", 40)) / 2, y, 40, YELLOW);
    y += 70;

    const char *lines[] = {TUTORIAL_DIALOGS};

    int numLines = sizeof(lines) / sizeof(lines[0]);
    for (int i = 0; i < numLines; i++) {
        DrawText(lines[i],
                 (screenWidth - MeasureText(lines[i], fontSize)) / 2,
                 y,
                 fontSize,
                 WHITE);
        y += 40;
    }

            DrawText( "Clique ou pressione ENTER para continuar",
                      (screenWidth - MeasureText("Clique ou pressione ENTER para continuar", 24)) / 2,
                       screenHeight - 80, 
                       24, 
                       LIGHTGRAY );
}

void initMenu(Menu *menu){
	menu->background = LoadTexture("assets/reino.png");

	float screenHeight = GetScreenHeight();
	float screenWidth = GetScreenWidth();
	float fundoHeight = screenHeight - hudHeight;

	createButton(&menu->buttonGame,GAME,BTN_START_COR,BTN_START_COR_HOVER,BTN_START_LINES_COR,false, "Start",(Rectangle){BTN_START_X,BTN_START_Y,BTN_START_WIDTH,BTN_START_HEIGHT},BTN_START_FONTE);

	createButton(&menu->buttonShop,SHOP,BTN_LOJA_COR,BTN_LOJA_COR_HOVER, BTN_LOJA_LINES_COR,true, "loja", (Rectangle){BTN_LOJA_X,BTN_LOJA_Y,BTN_LOJA_WIDTH,BTN_LOJA_HEIGHT},BTN_LOJA_FONTE);


}

State updateMenuScreen(void *data){
	Menu *menu = (Menu *)data;
	if(menu->background.id == -1) initMenu(menu);

	State state = KEEP;

	if(IsKeyPressed(KEY_ESCAPE)) return SAVE;

	if((state = updateButton(&menu->buttonGame)) != KEEP) return state;
	if((state = updateButton(&menu->buttonShop)) != KEEP) return state;

	return state;
};

void drawMenuScreen(void *data){
	const Menu *menu = (Menu *)data;

	int screenHeight = GetScreenHeight();
	int screenWidth = GetScreenWidth();

	float fundoHeight = screenHeight - hudHeight;
	float scaleX = (float)screenWidth / menu->background.width;
	float scaleY = fundoHeight / menu->background.height;
	float scale = (scaleX > scaleY) ? scaleX : scaleY;

	desenharReino(menu->background, fundoHeight, scale, screenWidth);

	desenharRetangulo(fundoHeight, screenWidth);

	Vector2 posiçãorei;

	desenharRei(*menu->king,posiçãorei, fundoHeight, screenWidth);

	DrawSideHUDBig(menu->player->health,menu->player->food,menu->player->power);

	RankingHUD(screenHeight);

	writeLevel(screenWidth);

	drawButton(&menu->buttonGame);
	drawButton(&menu->buttonShop);

	DrawGoldHUDAt(&goldHUD, 20, 20);

};

void drawButton(const Button *button){

    if(!button->isRounded){
        DrawRectangleRec(button->dimensions, button->isHover ? button->color : button->hoverColor);
        DrawRectangleLinesEx(button->dimensions, 3, button->lines);
    }else{
        DrawRectangleRounded(button->dimensions, 0.2f, 8, button->isHover ? button->color : button->hoverColor);
        DrawRectangleRoundedLines(button->dimensions, 0.2f, 8, button->lines);
    }

    int textoLargura = MeasureText(button->text, button->font);

    int textoX = button->dimensions.x + (button->dimensions.width - textoLargura) / 2;
    int textoY = button->dimensions.y + (button->dimensions.height - button->font) / 2 + 2;

    DrawText(  button->text,
                textoX,
                textoY, 
                button->font, 
                WHITE );
}

State updateButton(Button *button){
	Vector2 mousePos = GetMousePosition();
	button->isHover = CheckCollisionPointRec(mousePos, button->dimensions);

	if(button->isHover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return button->whereToGo;
	return KEEP;
};

void createButton(Button *button,State whereToGo,Color color,Color colorHover,Color lines,bool isRounded,const char *text,Rectangle dimensions,int font){

	button->color = color;
	button->hoverColor = colorHover;
	button->isHover = false;
	button->isRounded = isRounded;
	button->whereToGo = whereToGo;
	button->dimensions = dimensions;
	button->font = font;
	button->lines = lines;
	strcpy(button->text, text);

};


