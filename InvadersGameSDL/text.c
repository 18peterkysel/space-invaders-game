#include <string.h>

#include "text.h"
#include "config.h"

Text* createText(SDL_Renderer* renderer, TTF_Font* font, SDL_Color color, char* text, int x, int y) {

	Text* b = (Text*)malloc(sizeof(Text));

	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, color);
	b->rect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	b->rect->w = textSurface->w;
	b->rect->h = textSurface->h;
	b->rect->x = x - (b->rect->w / 2);
	b->rect->y = y;

	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	b->texture = textTexture;

	b->buttonText = (char*)malloc((strlen(text) + 1) * sizeof(char));
	b->buttonText = text;

	SDL_FreeSurface(textSurface);
	return b;
}

void destroyText(SDL_Renderer* renderer, Text* text) {
	SDL_DestroyTexture(text->texture);
	free(text->rect);
	free(text);
}

void renderText(SDL_Renderer* renderer, char* rawText, TTF_Font* font, SDL_Color color, int offset, int delay) {
	SDL_RenderClear(renderer);

	Text* text = (Text*)malloc(sizeof(Text));
	text = createText(renderer, font, color, rawText, SCREEN_WIDTH / 2, offset);

	SDL_RenderCopy(renderer, text->texture, NULL, text->rect);
	SDL_RenderPresent(renderer);
	SDL_Delay(delay);

	destroyText(renderer, text);
}