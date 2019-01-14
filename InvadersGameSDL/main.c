#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_TTF.h>
#include <SDL_mixer.h>

#include "config.h"
#include "text.h"
#include "menu.h"
#include "entity.h"
#include "ship.h"
#include "aliens.h"
#include "bullet.h"
#include "soundeffects.h"
#include "animation.h"

// Define background color in R, G, B
#define R 0
#define G 0
#define B 0

// Support functions
SDL_Window* initializeSdl()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		exit(-1);
	}

	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		exit(-1);
	}

	if (TTF_Init() == -1) {
		printf("SDL_TTF could not initialize! TTF_Init: %s\n", TTF_GetError());
		exit(-1);
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		exit(-1);
	}

	SDL_Window* window = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (window == NULL) {
		printf("Window cannot be NULL!");
		exit(-1);
	}

	return window;
}

void render(SDL_Renderer* renderer,
	Entity* ship,
	Entity* aliens, int alienCount,
	Animation* animations, int animationCount,
	Object* shipBullets, int shipBulletCount,
	Object* alienBullets, int alienBulletCount) {
	// Set background color to renderer, copy to renderer ship texture with new postion and present renderer
	// SDL_SetRenderDrawColor(renderer, R, G, B, 255);
	SDL_RenderClear(renderer);

	if (ship)
		SDL_RenderCopy(renderer, ship->object->imageText, NULL, ship->object->imageRect);

	for (int i = 0; i < alienCount; i++)
		SDL_RenderCopy(renderer, aliens[i].object->imageText, NULL, aliens[i].object->imageRect);

	for (int i = 0; i < animationCount; i++)
		SDL_RenderCopy(renderer, animations[i].object->imageText, animations[i].srcRect, animations[i].object->imageRect);

	for (int i = 0; i < shipBulletCount; i++)
		SDL_RenderCopy(renderer, shipBullets[i].imageText, NULL, shipBullets[i].imageRect);

	for (int i = 0; i < alienBulletCount; i++)
		SDL_RenderCopy(renderer, alienBullets[i].imageText, NULL, alienBullets[i].imageRect);

	SDL_RenderPresent(renderer);
}

void renderTextLevel(SDL_Renderer* renderer, int levelNumber) {
	TTF_Font* font = TTF_OpenFont(GET_RESOURCE_PATH("Agrem.ttf"), 64);
	SDL_Color whiteColor = WHITE_COLOR;

	static const int lvlCompleteDelay = 1000;
	static const int lnlNextDelay = 100; // reinitializing arrays takes a long enough time by itself
	if (levelNumber == 1) {
		renderText(renderer, "Level one", font, whiteColor, SCREEN_VERTICAL_OFFSET, lnlNextDelay);
	}
	else if (levelNumber == 2) {
		renderText(renderer, "Level one Complete", font, whiteColor, SCREEN_VERTICAL_OFFSET, lvlCompleteDelay);
		renderText(renderer, "Level two", font, whiteColor, SCREEN_VERTICAL_OFFSET, lnlNextDelay);
	}
	else {
		renderText(renderer, "Level two Complete", font, whiteColor, SCREEN_VERTICAL_OFFSET, lvlCompleteDelay);
		renderText(renderer, "Level three", font, whiteColor, SCREEN_VERTICAL_OFFSET, lnlNextDelay);
	}
}

void initLevel(int levelNumber, SDL_Renderer* renderer, Entity** ship, int* shipCount, Entity** aliens, int* alienCount) {
	if (!*ship) {
		*ship = createShip(renderer, *ship);
		*shipCount = 1;
	}

	int alienVerticalCount = 0;
	int alienHorizontalCount = 0;

	switch (levelNumber) {
	case 1:
		alienVerticalCount = 2;
		alienHorizontalCount = 10;
		break;
	case 2:
		alienVerticalCount = 3;
		alienHorizontalCount = 13;
		break;
	case 3:
		alienVerticalCount = 4;
		alienHorizontalCount = 16;
		break;
	default:
		alienVerticalCount = 1;
		alienHorizontalCount = 1;
	}
	*aliens = createAliens(renderer, *aliens, alienVerticalCount, alienHorizontalCount);
	*alienCount = alienVerticalCount * alienHorizontalCount;
}

void evaluateBulletCollisions(
	SDL_Renderer* renderer,
	Object** bullets, int* bulletCount,
	Entity** entities, int* entitiesCount,
	Animation** animations, int* animationCount,
	SoundEffect* effects) {
	int destroyBulletsCount = 0;
	int destroyEntitiesCount = 0;
	Object** destroyBullets = (Object**)malloc(sizeof(Object*));
	Entity** destroyEntities = (Entity**)malloc(sizeof(Entity*));

	for (int i = 0; i < *bulletCount; i++) {
		Object* bullet = (*bullets) + i;
		for (int j = 0; j < *entitiesCount; j++) {
			Entity* entity = (*entities) + j;
			if (SDL_HasIntersection(bullet->imageRect, entity->object->imageRect)) {
				// TODO: Check if alien is already add to destAliens array
				//       - should not be necessary, should be done for bullets though
				entity->health -= 10;
				if (entity->health <= 0) {
					//					if (strcmp(entity->object->imageName, GET_RESOURCE_PATH("shipInvaders.png")) == 0) {
					Mix_PlayChannel(-1, effects->explosion, 0);
					//					}
					//					else
					//					{
					//						Mix_PlayChannel(-1, effects->invaderkilled, 0);
					//					}
					destroyEntities = (Entity**)realloc(destroyEntities, (destroyEntitiesCount + 1) * sizeof(Entity*));
					destroyEntities[destroyEntitiesCount] = entity;
					destroyEntitiesCount++;
					addExplosion(renderer, entity->object->imageRect, animations, animationCount);
				}

				destroyBullets = (Object**)realloc(destroyBullets, (destroyBulletsCount + 1) * sizeof(Object*));
				destroyBullets[destroyBulletsCount] = bullet;
				destroyBulletsCount++;
				break;
			}
		}
		if (bullet->imageRect->y <= 0 || bullet->imageRect->y >= SCREEN_HEIGHT) {
			destroyBullets = (Object**)realloc(destroyBullets, (destroyBulletsCount + 1) * sizeof(Object*));
			destroyBullets[destroyBulletsCount] = bullet;
			destroyBulletsCount++;
		}
	}


	for (int i = 0; i < destroyBulletsCount; i++) {
		destroyBullet(bullets, destroyBullets[i], bulletCount);
	}
	for (int i = 0; i < destroyEntitiesCount; i++) {
		destroyEntity(entities, destroyEntities[i], entitiesCount);
	}
}

void evaluateMovementCollision(SDL_Renderer* renderer, Entity** ship, int* shipCount,
	Entity** aliens, int* alienCount, Animation** animations, int* animationCount, SoundEffect* effects) {
	if (*alienCount <= 0 || *shipCount <= 0 || 
		((*aliens)[*alienCount - 1].object->imageRect->y <= PLAYER_VERTICAL_POSITION - ENTITY_SIZE)) {
		return;
	}
	for (int i = 0; i < *alienCount; i++) {
		Entity* alien = (*aliens) + i;
		if (SDL_HasIntersection((*ship)->object->imageRect, alien->object->imageRect)) {
			(*ship)->health = 0; // -= 10 means it would be possible for some aliens to runaway and we'd have to check for it
			if ((*ship)->health <= 0) {
				addExplosion(renderer, (*ship)->object->imageRect, animations, animationCount);
				destroyEntity(ship, *ship, shipCount);
				Mix_PlayChannel(-1, effects->explosion, 0);
			}
			addExplosion(renderer, alien->object->imageRect, animations, animationCount);
			destroyEntity(aliens, alien, alienCount);
			Mix_PlayChannel(-1, effects->invaderkilled, 0);
			break;
		}
	}
}

void cleanUp(
	Entity* ship, int* shipCount,
	Entity* aliens, int* alienCount,
	Animation* animations, int* animationCount,
	Object* ship_bullets, int* shipBulletCount,
	Object* alien_bullets, int* alienBulletCount
) {
	if (ship) {
		destroyEntity(&ship, ship, shipCount);
	}
	aliens = clearAliens(aliens, alienCount);
	animations = clearAnimations(animations, animationCount);
	while (*shipBulletCount != 0) {
		destroyBullet(&ship_bullets, &ship_bullets[*shipBulletCount - 1], shipBulletCount);
	}
	while (*alienBulletCount != 0) {
		destroyBullet(&alien_bullets, &alien_bullets[*alienBulletCount - 1], alienBulletCount);
	}
}

void gameloop(SDL_Renderer* renderer) {
	SDL_Event event;
	const Uint8* keystate;
	int alienDirection = ALIEN_DIRECTION_RIGHT;
	int gameState = GAME_STATE_MENU;
	int menuState = MENU_STATE_WELCOME;
	int levelNumber = -1;

	int shipCount = -1;
	Entity* ship = NULL; // = createShip(renderer, NULL);

	int alienCount = -1;
	Entity* aliens = NULL; // = createAliens(renderer, NULL);

	int animationCount = 0;
	Animation* animations = NULL;

	int shipBulletCount = 0;
	Object* ship_bullets = (Object*)malloc(0 * sizeof(Object));

	int alienBulletCount = 0, lastAlienFiringTime = 0;
	Object* alien_bullets = (Object*)malloc(0 * sizeof(Object));

	SoundEffect* effects = initSoundEffects();

	// menu
	Menu* menu = initMenu();
	createMenuState(renderer, menu, menuState);

	int selected = MENU_SELECTED_PLAY;
	bool keyDownHappened = true;

	// variable time-step
	int currentTicks = -1, elapsedTicks = -1, accumulatedTicks = 0, lastTicks = SDL_GetTicks();
	int lastShipFiringTime = 0;

	while (gameState != GAME_STATE_QUIT) {
		// variable time-step
		currentTicks = SDL_GetTicks();
		elapsedTicks = currentTicks - lastTicks;
		accumulatedTicks += elapsedTicks;
		lastTicks = currentTicks;
		lastShipFiringTime += accumulatedTicks;
		lastAlienFiringTime += accumulatedTicks;

		if (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				gameState = GAME_STATE_QUIT;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
				case SDLK_q:
					gameState = GAME_STATE_QUIT;
					break;
				}
				keyDownHappened = true;
				break;
			}
		}

		if (accumulatedTicks >= TIME_STEP) {
			switch (gameState) {
			case GAME_STATE_PLAY:
				keystate = SDL_GetKeyboardState(NULL);

				if (ship) {
					handleShipMovement(ship, keystate, &ship_bullets, &shipBulletCount, renderer, &lastShipFiringTime, effects);
				}
				else {
					gameState = GAME_STATE_MENU;
					menuState = MENU_STATE_GAME_OVER;
					createMenuState(renderer, menu, menuState);
				}

				if (alienCount > 0) {
					moveAliens(aliens, alienCount, &alienDirection);

					if (lastAlienFiringTime >= ALIEN_FIRING_DELAY) {
						fireBullet(renderer, aliens, &alien_bullets, &alienBulletCount, true, &alienCount);
						lastAlienFiringTime = 0;
					}
				}
				else {
					while (shipBulletCount != 0) {
						destroyBullet(&ship_bullets, &ship_bullets[shipBulletCount - 1], &shipBulletCount);
					}
					while (alienBulletCount != 0) {
						destroyBullet(&alien_bullets, &alien_bullets[alienBulletCount - 1], &alienBulletCount);
					}

					if (levelNumber < LEVEL_COUNT) {
						levelNumber++;
						renderTextLevel(renderer, levelNumber);
						initLevel(levelNumber, renderer, &ship, &shipCount, &aliens, &alienCount);
						// first render takes a long time - dirty workaround for the initial lag
						render(renderer, ship, aliens, alienCount, animations, animationCount, ship_bullets, shipBulletCount, alien_bullets, alienBulletCount);
						lastTicks = SDL_GetTicks();
					}
					else
					{
						gameState = GAME_STATE_MENU;
						menuState = MENU_STATE_VICTORY;
						createMenuState(renderer, menu, menuState);
					}
				}

				advanceAnimations(&animations, &animationCount);

				moveBullets(alien_bullets, alienBulletCount, ship_bullets, shipBulletCount);
				evaluateBulletCollisions(renderer, &ship_bullets, &shipBulletCount, &aliens, &alienCount, &animations, &animationCount, effects);
				evaluateBulletCollisions(renderer, &alien_bullets, &alienBulletCount, &ship, &shipCount, &animations, &animationCount, effects);

				evaluateMovementCollision(renderer, &ship, &shipCount, &aliens, &alienCount, &animations, &animationCount, effects);

				render(renderer, ship, aliens, alienCount, animations, animationCount, ship_bullets, shipBulletCount, alien_bullets, alienBulletCount);
				break;
			case GAME_STATE_MENU:
				if (menuState == MENU_STATE_GAME_OVER || menuState == MENU_STATE_WELCOME || menuState == MENU_STATE_VICTORY) {
					levelNumber = 0;
					if (menuState == MENU_STATE_GAME_OVER || menuState == MENU_STATE_VICTORY) {
						while (shipBulletCount != 0) {
							destroyBullet(&ship_bullets, &ship_bullets[shipBulletCount - 1], &shipBulletCount);
						}
						while (alienBulletCount != 0) {
							destroyBullet(&alien_bullets, &alien_bullets[alienBulletCount - 1], &alienBulletCount);
						}
						if (alienCount != 0) {
							aliens = clearAliens(aliens, &alienCount);
							alienCount = 0;
						}
						if (animationCount != 0) {
							animations = clearAnimations(animations, &animationCount);
						}
						if (shipCount != 0) {
							destroyEntity(&ship, ship, &shipCount);
						}
					}
				}

				if (keyDownHappened) {
					keyDownHappened = false;
					switch (event.key.keysym.sym) {
					case SDLK_UP:
						selected = 0;
						break;
					case SDLK_DOWN:
						selected = 1;
						break;
					case SDLK_RETURN:
					case SDLK_KP_ENTER:
						gameState = selected == MENU_SELECTED_PLAY ? GAME_STATE_PLAY : GAME_STATE_QUIT;
						if (gameState == GAME_STATE_PLAY) {
							levelNumber++;
							renderTextLevel(renderer, levelNumber);
							initLevel(levelNumber, renderer, &ship, &shipCount, &aliens, &alienCount);

							// first render takes a long time - dirty workaround for the initial lag
							render(renderer, ship, aliens, alienCount, animations, animationCount, ship_bullets, shipBulletCount, alien_bullets, alienBulletCount);
							lastTicks = SDL_GetTicks();
							continue;
						}
					}
				}
				renderMenu(renderer, menu, selected);
				break; // case GAME_STATE_MENU
			}
			//accumulatedTicks -= TIME_STEP;
			accumulatedTicks = 0;
		}
		else {
			SDL_Delay(DELAY);
		}
	}
	cleanUp(ship, &shipCount, aliens, &alienCount, animations, &animationCount, ship_bullets, &shipBulletCount, alien_bullets, &alienBulletCount);
}

int main(int argc, char* args[])
{
	// Init window and PNG images loading
	SDL_Window* window = initializeSdl();
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	gameloop(renderer);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}