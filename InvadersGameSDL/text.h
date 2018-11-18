#pragma once

#include <SDL.h>
#include <SDL_TTF.h>

typedef struct {
	SDL_Texture* texture;
	SDL_Rect* rect;
	char* buttonText;
} Text;

Text* createText(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, char* text, int x, int y);

void destroyText(SDL_Renderer* renderer, Text* text);

void renderText(SDL_Renderer* renderer, char* rawText, TTF_Font* font, SDL_Color color, int offset, int delay);
