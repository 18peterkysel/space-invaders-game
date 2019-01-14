#include <stdbool.h>
#include "aliens.h"
#include "config.h"
#include "assert.h"

Entity* createAliens(SDL_Renderer* renderer, Entity* aliens, int verticalCount, int horizontalCount) {
	assert (verticalCount <= 4);
	int alienCount = verticalCount * horizontalCount;
	if (aliens == NULL) {
		aliens = (Entity*)malloc(alienCount * sizeof(Entity));
	}

	// Create aliens
	for (int i = 0; i < alienCount; i++) {
		aliens[i].object = (Object*)malloc(sizeof(Object));
		aliens[i].object->imageRect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
		aliens[i].health = ALIEN_HEALTH;
	}

	char imageNames[][30] = {
		{GET_RESOURCE_PATH("alien_easy.png")},
		{GET_RESOURCE_PATH("alien_medium.png")},
		{GET_RESOURCE_PATH("alien_hard.png")},
		{GET_RESOURCE_PATH("alien_boss.png")}
	};


	int rowOffset = 0;
	// Set texture, position and size of aliens
	for (int row = 0; row < verticalCount; row++) {
		int indexOffset = row * horizontalCount;
		int colOffset = 2;
		rowOffset += (ENTITY_SIZE + 2);
		for (int col = 0; col < horizontalCount; col++) {
			// Set texture of alien image
			aliens[indexOffset + col].object->imageName = imageNames[row];
			aliens[indexOffset + col].object->imageText = IMG_LoadTexture(renderer, aliens[indexOffset + col].object->imageName);

			// Set position of each alien
			aliens[indexOffset + col].object->imageRect->x = colOffset;
			aliens[indexOffset + col].object->imageRect->y = rowOffset;

			// Set size of each alien
			aliens[indexOffset + col].object->imageRect->w = ENTITY_SIZE;
			aliens[indexOffset + col].object->imageRect->h = ENTITY_SIZE;

			// Make small space between aliens => + 2
			colOffset += ENTITY_SIZE + 2;
		}
	}

	return aliens;
}

Entity* clearAliens(Entity* aliens, int* aliensCount) {
	for (int i = 0; i < *aliensCount; i++) {
		SDL_DestroyTexture(aliens[i].object->imageText);
		free(aliens[i].object->imageRect);
		free(aliens[i].object);
	}
	free(aliens);
	*aliensCount = 0;
	return NULL;
}

int moveAliens(Entity* aliens, int alienCount, int* direction) {
	bool edgeCollision = false;
	for (int i = 0; i < alienCount; i++) {
		SDL_Rect* alienPos = aliens[i].object->imageRect;
		if ((*direction) == ALIEN_DIRECTION_RIGHT) {
			if (alienPos->x + ALIEN_HORIZONTAL_SPEED > SCREEN_WIDTH - ENTITY_SIZE) {
				edgeCollision = true;
				(*direction) = ALIEN_DIRECTION_LEFT;
			}
		}
		else {
			if (alienPos->x - ALIEN_HORIZONTAL_SPEED < 0) {
				edgeCollision = true;
				(*direction) = ALIEN_DIRECTION_RIGHT;
			}
		}
	}
	if (edgeCollision) {
		for (int i = 0; i < alienCount; i++) {
			SDL_Rect* alienPos = aliens[i].object->imageRect;
			alienPos->y += ALIEN_VERTICAL_SPEED;
		}
	}
	else {
		int moveBy = ALIEN_HORIZONTAL_SPEED * (*direction);
		for (int i = 0; i < alienCount; i++) {
			SDL_Rect* alienPos = aliens[i].object->imageRect;
			alienPos->x += moveBy;
		}
	}

	return 0;
}
