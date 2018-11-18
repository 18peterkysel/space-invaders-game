#include "menu.h"
#include "config.h"

Menu* initMenu() {
	Menu* menu = (Menu*)malloc(sizeof(Menu));

	menu->headerFont = TTF_OpenFont(GET_RESOURCE_PATH("Agrem.ttf"), 64);
	menu->buttonTextFont = TTF_OpenFont(GET_RESOURCE_PATH("Agrem.ttf"), 48);
	menu->menuButtons = NULL;
	menu->countButton = -1;

	return menu;
}

void renderMenu(SDL_Renderer* renderer, Menu* menu, int selected) {
	SDL_RenderClear(renderer);

	SDL_RenderCopy(renderer, menu->header->texture, NULL, menu->header->rect);
	for (int i = 0; i < menu->countButton; i++) {
		SDL_RenderCopy(renderer, menu->menuButtons[i]->texture, NULL, menu->menuButtons[i]->rect);
	}

	char* namePlayButton = menu->menuButtons[0]->buttonText;
	for (int i = 0; i < menu->countButton; i++) {
		SDL_DestroyTexture(menu->menuButtons[i]->texture);
		free(menu->menuButtons[i]->rect);
		free(menu->menuButtons[i]);
	}

	SDL_Color textColorWhite = { 255,255,255 };
	SDL_Color textColorRed = { 240,15,15 };

	int offset = SCREEN_HEIGHT / 4;
	int indexButton = selected == MENU_SELECTED_PLAY ? 0 : 1;

	if (indexButton == 0) {
		menu->menuButtons[0] = createText(renderer, menu->buttonTextFont, textColorRed, namePlayButton, SCREEN_WIDTH / 2, offset * 2);
		menu->menuButtons[1] = createText(renderer, menu->buttonTextFont, textColorWhite, "Quit", SCREEN_WIDTH / 2, offset * 3);
	}
	else
	{
		menu->menuButtons[0] = createText(renderer, menu->buttonTextFont, textColorWhite, namePlayButton, SCREEN_WIDTH / 2, offset * 2);
		menu->menuButtons[1] = createText(renderer, menu->buttonTextFont, textColorRed, "Quit", SCREEN_WIDTH / 2, offset * 3);
	}
	SDL_RenderPresent(renderer);
}

void destroyMenu(Menu* menu) {
	for (int i = 0; i < menu->countButton; i++) {
		SDL_DestroyTexture(menu->menuButtons[i]->texture);
		free(menu->menuButtons[i]->rect);
		free(menu->menuButtons[i]);
	}
	free(menu->menuButtons);
	menu->countButton = -1;
}


void createMenuState(SDL_Renderer* renderer, Menu* menu, int menuState) {
	if (menu->menuButtons != NULL) {
		destroyMenu(menu);
	}

	int offset = SCREEN_HEIGHT / 4;
	SDL_Color redColor = RED_COLOR;
	SDL_Color whiteColor = WHITE_COLOR;

	menu->countButton = 2;

	switch (menuState) {
	case MENU_STATE_WELCOME:
		menu->header = (Text*)malloc(sizeof(Text));
		menu->header = createText(renderer, menu->headerFont, redColor, "Space Invaders", SCREEN_WIDTH / 2, offset);

		menu->menuButtons = (Text**)malloc(menu->countButton * sizeof(Text*));
		menu->menuButtons[0] = createText(renderer, menu->buttonTextFont, redColor, "Play", SCREEN_WIDTH / 2, offset * 2);
		menu->menuButtons[1] = createText(renderer, menu->buttonTextFont, whiteColor, "Quit", SCREEN_WIDTH / 2, offset * 3);
		break;
	case MENU_STATE_GAME_OVER:
		menu->header = (Text*)malloc(sizeof(Text));
		menu->header = createText(renderer, menu->headerFont, redColor, "Game over", SCREEN_WIDTH / 2, offset);

		menu->menuButtons = (Text**)malloc(menu->countButton * sizeof(Text*));
		menu->menuButtons[0] = createText(renderer, menu->buttonTextFont, redColor, "Replay", SCREEN_WIDTH / 2, offset * 2);
		menu->menuButtons[1] = createText(renderer, menu->buttonTextFont, whiteColor, "Quit", SCREEN_WIDTH / 2, offset * 3);
		break;
	case MENU_STATE_VICTORY:
		menu->header = (Text*)malloc(sizeof(Text));
		menu->header = createText(renderer, menu->headerFont, redColor, "Congratulations", SCREEN_WIDTH / 2, offset);

		menu->menuButtons = (Text**)malloc(menu->countButton * sizeof(Text*));
		menu->menuButtons[0] = createText(renderer, menu->buttonTextFont, redColor, "Replay", SCREEN_WIDTH / 2, offset * 2);
		menu->menuButtons[1] = createText(renderer, menu->buttonTextFont, whiteColor, "Quit", SCREEN_WIDTH / 2, offset * 3);
		break;
	}
}