#pragma once

#include "entity.h"
#include "effects.h"

Entity* createShip(SDL_Renderer* renderer, Entity* ship);

void handleShipMovement(Entity* ship, const Uint8 *keystate, Object** bullets, int* bulletCount, SDL_Renderer* renderer, int* lastFiringTime, Effects* effects);
