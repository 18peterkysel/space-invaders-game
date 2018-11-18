#include <stdbool.h>
#include <SDL.h>
#include <SDL_image.h>

#include "ship.h"
#include "config.h"
#include "bullet.h"

Entity* createShip(SDL_Renderer* renderer, Entity* ship) {
	// Create ship
	if (ship == NULL) {
		ship = (Entity*)malloc(sizeof(Entity));
	}
	ship->health = PLAYER_HEALTH;

	// Set image name
	ship->object = (Object*)malloc(sizeof(Object));
	ship->object->imageName = GET_RESOURCE_PATH("shipInvaders.png");

	// Set image texture
	ship->object->imageText = IMG_LoadTexture(renderer, ship->object->imageName);

	// Set ship position and size
	ship->object->imageRect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	ship->object->imageRect->x = SCREEN_WIDTH / 2;
	ship->object->imageRect->y = SCREEN_HEIGHT - (ENTITY_SIZE * 2);
	ship->object->imageRect->w = ENTITY_SIZE;
	ship->object->imageRect->h = ENTITY_SIZE;

	return ship;
}

void handleShipMovement(Entity* ship, const Uint8 *keystate, Object** bullets, int* bulletCount, SDL_Renderer* renderer, int* lastFiringTime, Effects* effects) {
	int moveBy = PLAYER_SPEED;
	if (keystate[SDL_SCANCODE_LEFT]) {
		ship->object->imageRect->x -= moveBy;
	}
	if (keystate[SDL_SCANCODE_RIGHT]) {
		ship->object->imageRect->x += moveBy;
	}
	if (keystate[SDL_SCANCODE_UP]) {
		//ship->imageRect->y += moveBy;
	}
	if (keystate[SDL_SCANCODE_DOWN]) {
		//ship->imageRect->y -= moveBy;
	}
	if (keystate[SDL_SCANCODE_SPACE]) {
		if ((*lastFiringTime) >= PLAYER_FIRING_DELAY) {
			Mix_PlayChannel(-1, effects->shoot, 0);
			fireBullet(renderer, ship, bullets, bulletCount, false, NULL);
			(*lastFiringTime) = 0;
		}
	}

	// Collide with edges of screen
	if (ship->object->imageRect->x < 0) {
		ship->object->imageRect->x = 0;
	}
	else if (ship->object->imageRect->x > SCREEN_WIDTH - ENTITY_SIZE) {
		ship->object->imageRect->x = SCREEN_WIDTH - ENTITY_SIZE;
	}
	if (ship->object->imageRect->y < 0) {
		ship->object->imageRect->y = 0;
	}
	else if (ship->object->imageRect->y > SCREEN_HEIGHT - ENTITY_SIZE) {
		ship->object->imageRect->y = SCREEN_HEIGHT - ENTITY_SIZE;
	}
}
