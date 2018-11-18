#include "bullet.h"
#include "config.h"

Object* createBullet(Entity* entity, SDL_Renderer* renderer, bool setAlienBullet, int* alienCount) {
	// Create bullet
	Object* bullet = (Object*)malloc(sizeof(Object));

	// Set rectangle
	bullet->imageRect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	int position;

	// Set bullet image name, position and size
	if (!setAlienBullet) {
		bullet->imageName = GET_RESOURCE_PATH("bullet_ship.png");
		bullet->imageRect->y = 0;
		position = 0;
	}
	else {
		bullet->imageName = GET_RESOURCE_PATH("bullet_alien.png");
		bullet->imageRect->y = ENTITY_SIZE;
		position = rand() % *alienCount;
	}

	// Set image texture
	bullet->imageText = IMG_LoadTexture(renderer, bullet->imageName);

	int x = entity[position].object->imageRect->x;
	int y = entity[position].object->imageRect->y;

	bullet->imageRect->x = x + (ENTITY_SIZE / 2) - (BULLET_SIZE_WIDTH / 2);
	bullet->imageRect->y += y;
	bullet->imageRect->w = BULLET_SIZE_WIDTH;
	bullet->imageRect->h = BULLET_SIZE_HEIGHT;

	return bullet;
}

void fireBullet(SDL_Renderer* renderer, Entity* entity, Object** bullets, int* bulletCount, bool setAlienBullet, int* enitiesCount) {
	// Realloc array of bullets
	(*bullets) = (Object*)realloc(*bullets, (*bulletCount + 1) * sizeof(Object));

	// Add bullet to array of bullets
	Object* bullet = createBullet(entity, renderer, setAlienBullet, enitiesCount);
	(*bullets)[*bulletCount] = *bullet;
	(*bulletCount)++;
}

void moveBullets(Object* alienBullets, int alienBulletCount, Object* shipBullets, int shipBulletCount) {
	for (int i = 0; i < shipBulletCount; i++) {
		shipBullets[i].imageRect->y -= (BULLET_SPEED);
	}
	for (int i = 0; i < alienBulletCount; i++) {
		alienBullets[i].imageRect->y += (BULLET_SPEED);
	}
}

void destroyBullet(Object** bullets, Object* bullet, int* bulletCount) {
	// Get postion of bullet to destroy
	int position = -1;
	for (int i = 0; i < *bulletCount; i++) {
		if (((*bullets) + i) == bullet) {
			position = i;
			break;
		}
	}

	if (position == -1) {
		printf("ERROR(destroyBullet): Cannot find address of bullet in array!\n");
		return;
	}

	SDL_DestroyTexture(bullet->imageText);
	free(bullet->imageRect);

	// Remove bullet from bullets array
	if (position != (*bulletCount - 1)) {
		for (; position < *bulletCount - 1; position++) {
			(*bullets)[position] = (*bullets)[position + 1];
		}
	}

	(*bulletCount)--;
	(*bullets) = (Object*)realloc(*bullets, *bulletCount * sizeof(Object));
}
