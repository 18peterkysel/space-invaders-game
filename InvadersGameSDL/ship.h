#pragma once

#include "entity.h"
#include "soundeffects.h"

Entity* createShip(SDL_Renderer* renderer, Entity* ship);

void handleShipMovement(Entity* ship, const Uint8 *keystate, Object** bullets, int* bulletCount, SDL_Renderer* renderer, int* lastFiringTime, SoundEffect* effects);
