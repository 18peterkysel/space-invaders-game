#pragma once

#include <SDL_mixer.h>

typedef struct {
	Mix_Chunk* shoot;
	Mix_Chunk* explosion;
	Mix_Chunk* invaderkilled;
} Effects;

Effects* initEffects();
