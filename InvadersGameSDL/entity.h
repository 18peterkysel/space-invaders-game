#pragma once

#include <SDL.h>

typedef struct {
	SDL_Texture* imageText;
	SDL_Rect* imageRect;
	char* imageName;
} Object;

typedef struct {
	Object* object;
	int health;
} Entity;

void destroyEntity(Entity** entities, Entity* entity, int* entitiesCount);
