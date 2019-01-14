#include "soundeffects.h"
#include "config.h"

SoundEffect* initSoundEffects() {
	SoundEffect* effects = (SoundEffect*)malloc(sizeof(SoundEffect));
	effects->shoot = Mix_LoadWAV(GET_RESOURCE_PATH("sounds/shoot.wav"));
	if (effects->shoot == NULL) {
		printf("Failed to load shoot sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		exit(-1);
	}
	effects->explosion = Mix_LoadWAV(GET_RESOURCE_PATH("sounds/explosion.wav"));
	if (effects->explosion == NULL) {
		printf("Failed to load explosion sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		exit(-1);
	}
	effects->invaderkilled = Mix_LoadWAV(GET_RESOURCE_PATH("sounds/invaderkilled.wav"));
	if (effects->invaderkilled == NULL) {
		printf("Failed to load invaderkilled sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		exit(-1);
	}

	// set volume of effects
	Mix_VolumeChunk(effects->shoot, 40);
	Mix_VolumeChunk(effects->explosion, 40);
	Mix_VolumeChunk(effects->invaderkilled, 40);

	return effects;
}