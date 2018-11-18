#pragma once

#include "text.h"

typedef struct {
	Text* header;
	TTF_Font* headerFont;
	Text** menuButtons;
	TTF_Font* buttonTextFont;
	int countButton;
} Menu;

Menu* initMenu();

void renderMenu(SDL_Renderer* renderer, Menu* menu, int selected);

void destroyMenu(Menu* menu);

void createMenuState(SDL_Renderer* renderer, Menu* menu, int menuState);
