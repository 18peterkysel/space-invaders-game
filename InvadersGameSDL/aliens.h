#pragma once

#include "entity.h"
#include <SDL.h>

Entity* createAliens(SDL_Renderer* renderer, Entity* aliens);

Entity* clearAliens(Entity* aliens, int aliensCount);

int moveAliens(Entity* aliens, int alienCount, int* direction);
