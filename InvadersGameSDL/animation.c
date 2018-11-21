#include "animation.h"

#include <stdio.h>
#include <SDL_image.h>


Animation* createExplosion(SDL_Renderer* renderer, SDL_Rect* position) {
	Animation* anim = (Animation*)malloc(sizeof(Animation));

	anim->object = (Object*)malloc(sizeof(Object));
	anim->object->imageName = GET_RESOURCE_PATH("explosion-alien.png");

	// Set image texture
	anim->object->imageText = IMG_LoadTexture(renderer, anim->object->imageName);

	anim->object->imageRect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	anim->object->imageRect->x = position->x;
	anim->object->imageRect->y = position->y;
	anim->object->imageRect->w = position->w;
	anim->object->imageRect->h = position->h;

	anim->srcRect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	anim->srcRect->x = 0;
	anim->srcRect->y = 0;
	anim->srcRect->w = ENTITY_SIZE;
	anim->srcRect->h = ENTITY_SIZE;

	return anim;
}

void addExplosion(SDL_Renderer* renderer, SDL_Rect* position, Animation** animations, int* animationCount) {
  	if (*animations == NULL) {
		(*animationCount)++;
		(*animations) = (Animation*)realloc(*animations, *animationCount * sizeof(Animation));
	}

	animations[*animationCount - 1] = createExplosion(renderer, position);
}

void destroyAnimation(Animation** animations, Animation* animation, int* animationCount) {
	// Get postion of bullet to destroy
	int position = -1;
	for (int i = 0; i < *animationCount; i++) {
		if (((*animations) + i) == animation) {
			position = i;
			break;
		}
	}

	if (position == -1) {
		printf("ERROR(destroyBullet): Cannot find address of bullet in array!\n");
		return;
	}

	SDL_DestroyTexture(animation->object->imageText);
	free(animation->object->imageRect);

	// Remove bullet from bullets array
	if (position != (*animationCount - 1)) {
		for (; position < *animationCount - 1; position++) {
			(*animations)[position] = (*animations)[position + 1];
		}
	}

	(*animationCount)--;
	(*animations) = (Animation*)realloc(*animations, *animationCount * sizeof(Animation));
}

void advanceAnimations(Animation** animations, int* animationCount) {
	if (*animationCount > 0) {
		int i;
		for (i = 0; i < *animationCount; i++) {
			Animation* currentAnimation = animations[i];
			currentAnimation->srcRect->x += ENTITY_SIZE;
			if (currentAnimation->srcRect->x > SPRITESHEET_SIZE) {
				currentAnimation->srcRect->x %= SPRITESHEET_SIZE;
				currentAnimation->srcRect->y += ENTITY_SIZE;
				if (currentAnimation->srcRect->y > SPRITESHEET_SIZE) {
					destroyAnimation(animations, currentAnimation, animationCount);
					i--;
				}
			}
		}
	}
}

Animation* clearAnimations(Animation* animations, int* animationCount) {
	for (int i = 0; i < *animationCount; i++) {
		SDL_DestroyTexture(animations[i].object->imageText);
		free(animations[i].object->imageRect);
		free(animations[i].object);
	}
	free(animations);
	*animationCount = 0;
	return NULL;
}
