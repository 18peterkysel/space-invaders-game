#pragma once
#include <SDL.h>

#include "config.h"
#include "entity.h"

typedef struct {
	SDL_Rect* srcRect;
	Object* object;
} Animation;

#define ANIMATION_SPRITE_COUNT 4
#define SPRITESHEET_SIZE ANIMATION_SPRITE_COUNT * ENTITY_SIZE

//Animation* createExplosion(SDL_Renderer* renderer, SDL_Rect* position);

void addExplosion(SDL_Renderer* renderer, SDL_Rect* position, Animation** animations, int* animationCount);

void advanceAnimations(Animation** animations, int* animationCount);

//void destroyAnimation(Animation** animations, Animation* animation, int* animationCount);

Animation* clearAnimations(Animation* animations, int* animationCount);
