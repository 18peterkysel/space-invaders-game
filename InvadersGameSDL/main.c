#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768
#define ENTITY_SIZE   30
#define BULLET_SIZE_WIDTH 50
#define BULLET_SIZE_HEIGHT 20

#define TIME_STEP 1000/10
#define DELAY 2

#define GAME_STATE_MENU 1
#define GAME_STATE_PLAY 2
#define GAME_STATE_PAUSE 3
#define GAME_STATE_QUIT 0

#define ALIEN_DIRECTION_RIGHT 1
#define ALIEN_DIRECTION_LEFT -1

#define ALIEN_VERTICAL_SPEED 40
#define ALIEN_HORIZONTAL_SPEED 8
#define ALIEN_HORIZONTAL_COUNT 4
#define ALIEN_VERTICAL_COUNT 1
#define ALIEN_COUNT (ALIEN_HORIZONTAL_COUNT * ALIEN_VERTICAL_COUNT)

#define PLAYER_SPEED 10
#define PLAYER_FIRING_DELAY 1000

#define BULLET_SPEED 15

#define GET_RESOURCE_PATH(res_name) "resources/"res_name

// Define background color in R, G, B
// #define R 0
// #define G 0
// #define B 0

// Creation of structs
typedef struct {
	SDL_Texture* imageText;
	SDL_Rect* imageRect;
	char* imageName;
} Object;

typedef struct {
	Object object;
	int health;
} Entity;

// Support functions
SDL_Window* init()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
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

	SDL_Window* window = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (window == NULL) {
		printf("Window cannot be NULL!");
		exit(-1);
	}

	return window;
}

Entity* createAliens(SDL_Renderer* renderer, int *alienCount) {
	*alienCount = ALIEN_COUNT;
	Entity* aliens = (Entity*)malloc(*alienCount * sizeof(Entity));

	// Create aliens
	for (int i = 0; i < 4; i++) {
		aliens[i].object.imageRect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
		aliens[i].health = 50;
	}

	// Set names of images to aliens
	aliens[0].object.imageName = GET_RESOURCE_PATH("alien_easy.png");
	aliens[1].object.imageName = GET_RESOURCE_PATH("alien_medium.png");
	aliens[2].object.imageName = GET_RESOURCE_PATH("alien_hard.png");
	aliens[3].object.imageName = GET_RESOURCE_PATH("alien_boss.png");

	// Set texture, position and size of aliens
	int offsetEntity = 2;
	for (int i = 0; i < *alienCount; i++) {
		// Set texture of alien image
		aliens[i].object.imageText = IMG_LoadTexture(renderer, aliens[i].object.imageName);

		// Set postion of each alien
		aliens[i].object.imageRect->x = offsetEntity;
		aliens[i].object.imageRect->y = 0;

		// Set size of each alien
		aliens[i].object.imageRect->w = ENTITY_SIZE;
		aliens[i].object.imageRect->h = ENTITY_SIZE;

		// Make small space between aliens => + 2
		offsetEntity += ENTITY_SIZE + 2;
	}
	return aliens;
}

Entity* createShip(SDL_Renderer* renderer) {
	// Create ship
	Entity* ship = (Entity*)malloc(sizeof(Entity));
	ship->health = 50;

	// Set image name
	ship->object.imageName = GET_RESOURCE_PATH("shipInvaders.png");

	// Set image texture
	ship->object.imageText = IMG_LoadTexture(renderer, ship->object.imageName);

	// Set ship position and size
	ship->object.imageRect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	ship->object.imageRect->x = SCREEN_WIDTH / 2;
	ship->object.imageRect->y = SCREEN_HEIGHT - (ENTITY_SIZE * 2);
	ship->object.imageRect->w = ENTITY_SIZE;
	ship->object.imageRect->h = ENTITY_SIZE;

	return ship;
}

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
	// TODO: if alien bullet, image texture is NULL
	bullet->imageText = IMG_LoadTexture(renderer, bullet->imageName);

	int x = entity[position].object.imageRect->x;
	int y = entity[position].object.imageRect->y;

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

int moveAliens(Entity* aliens, int alienCount, int* direction) {
	bool edgeCollision = false;
	for (int i = 0; i < alienCount; i++) {
		SDL_Rect* alienPos = aliens[i].object.imageRect;
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
			SDL_Rect* alienPos = aliens[i].object.imageRect;
			alienPos->y += ALIEN_VERTICAL_SPEED;
		}
	}
	else {
		int moveBy = ALIEN_HORIZONTAL_SPEED * (*direction);
		for (int i = 0; i < alienCount; i++) {
			SDL_Rect* alienPos = aliens[i].object.imageRect;
			alienPos->x += moveBy;
		}
	}

	return 0;
}

void moveBullets(Object* alienBullets, int alienBulletCount, Object* shipBullets, int shipBulletCount) {
	for (int i = 0; i < shipBulletCount; i++) {
		shipBullets[i].imageRect->y -= (BULLET_SPEED);
	}
	for (int i = 0; i < alienBulletCount; i++) {
		alienBullets[i].imageRect->y += (BULLET_SPEED);
	}
}

void handleShipMovement(Entity* ship, const Uint8 *keystate, Object** bullets, int* bulletCount, SDL_Renderer* renderer, int* lastFiringTime) {
	int moveBy = PLAYER_SPEED;
	if (keystate[SDL_SCANCODE_LEFT]) {
		ship->object.imageRect->x -= moveBy;
	}
	if (keystate[SDL_SCANCODE_RIGHT]) {
		ship->object.imageRect->x += moveBy;
	}
	if (keystate[SDL_SCANCODE_UP]) {
		//ship->imageRect->y += moveBy;
	}
	if (keystate[SDL_SCANCODE_DOWN]) {
		//ship->imageRect->y -= moveBy;
	}
	if (keystate[SDL_SCANCODE_SPACE]) {
		if ((*lastFiringTime) >= PLAYER_FIRING_DELAY) {
			fireBullet(renderer, ship, bullets, bulletCount, false, NULL);
			(*lastFiringTime) = 0;
		}
	}

	// Collide with edges of screen
	if (ship->object.imageRect->x < 0) {
		ship->object.imageRect->x = 0;
	}
	else if (ship->object.imageRect->x > SCREEN_WIDTH - ENTITY_SIZE) {
		ship->object.imageRect->x = SCREEN_WIDTH - ENTITY_SIZE;
	}
	if (ship->object.imageRect->y < 0) {
		ship->object.imageRect->y = 0;
	}
	else if (ship->object.imageRect->y > SCREEN_HEIGHT - ENTITY_SIZE) {
		ship->object.imageRect->y = SCREEN_HEIGHT - ENTITY_SIZE;
	}
}

void render(SDL_Renderer* renderer, Entity* ship, Entity* aliens, int alienCount, Object* shipBullets, int shipBulletCount, Object* alienBullets, int alienBulletCount) {
	// Set background color to renderer, copy to renderer ship texture with new postion and present renderer
//	SDL_SetRenderDrawColor(renderer, R, G, B, 255);
	SDL_RenderClear(renderer);

	if (ship) {
		SDL_RenderCopy(renderer, ship->object.imageText, NULL, ship->object.imageRect);
	}

	for (int i = 0; i < alienCount; i++)
		SDL_RenderCopy(renderer, aliens[i].object.imageText, NULL, aliens[i].object.imageRect);

	for (int i = 0; i < shipBulletCount; i++)
		SDL_RenderCopy(renderer, shipBullets[i].imageText, NULL, shipBullets[i].imageRect);

	for (int i = 0; i < alienBulletCount; i++)
		SDL_RenderCopy(renderer, alienBullets[i].imageText, NULL, alienBullets[i].imageRect);

	SDL_RenderPresent(renderer);
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

	// TODO: Detroy resources and free memmory
	SDL_DestroyTexture(bullet->imageText);
	free(bullet->imageRect);
	// free(bullet->imageName);
	// free(bullet);

	// Remove bullet from bullets array
	if (position != (*bulletCount - 1)) {
		for (position; position < *bulletCount - 1; position++) {
			(*bullets)[position] = (*bullets)[position + 1];
		}
	}

	(*bulletCount)--;
	(*bullets) = (Object*)realloc(*bullets, *bulletCount * sizeof(Object));
}

void destroyEntity(Entity** entities, Entity* entity, int* entitiesCount) {
	// Get postion of entity to detroy
	int position = -1;
	for (int i = 0; i < *entitiesCount; i++) {
		if (&((*entities)[i]) == entity) {
			position = i;
			break;
		}
	}

	if (position == -1) {
		printf("ERROR(destroyAlien): Cannot find address of entity in array!\n");
		return;
	}

	// TODO: Detroy resources and free memmory
	SDL_DestroyTexture(entity->object.imageText);
	free(entity->object.imageRect);
	//free(alien->object.imageName);
	//free(&alien);

	// Remove entity from entities array
	if (position != (*entitiesCount - 1)) {
		for (position; position < *entitiesCount - 1; position++) {
			(*entities)[position] = (*entities)[position + 1];
		}
	}

	(*entitiesCount)--;
	(*entities) = (Entity*)realloc(*entities, *entitiesCount * sizeof(Entity));
}

void evaluateCollisions(Object** bullets, int* bulletCount, Entity** entities, int* enitiesCount) {
	//TODO: upravit funkciu kvoli duplicite kodu
	int destroyBulletsCount = 0;
	int destroyEntitiesCount = 0;
	Object** destroyBullets = (Object**)malloc(sizeof(Object*));;
	Entity** destroyEntities = (Entity**)malloc(sizeof(Entity*));;

	for (int i = 0; i < *bulletCount; i++) {
		for (int j = 0; j < *enitiesCount; j++) {
			if (SDL_HasIntersection((*bullets)[i].imageRect, (*entities)[j].object.imageRect)) {
				// TODO: Check if alien is already add to destAliens array
				//       - should not be necessary, should be done for bullets though
				(*entities)[j].health -= 10;
				if ((*entities)[j].health == 0) {
					destroyEntities = (Entity**)realloc(destroyEntities, (destroyEntitiesCount + 1) * sizeof(Entity*));
					destroyEntities[destroyEntitiesCount] = &((*entities)[j]);
					destroyEntitiesCount++;
				}

				destroyBullets = (Object**)realloc(destroyBullets, (destroyBulletsCount + 1) * sizeof(Object*));
				destroyBullets[destroyBulletsCount] = &((*bullets)[i]);
				destroyBulletsCount++;
				break;
			}
		}
		if ((*bullets)[i].imageRect->y <= 0 || (*bullets)[i].imageRect->y >= SCREEN_HEIGHT) {
			destroyBullets = (Object**)realloc(destroyBullets, (destroyBulletsCount + 1) * sizeof(Object*));
			destroyBullets[destroyBulletsCount] = &((*bullets)[i]);
			destroyBulletsCount++;
		}
	}


	for (int i = 0; i < destroyBulletsCount; i++) {
		destroyBullet(bullets, destroyBullets[i], bulletCount);
	}
	for (int i = 0; i < destroyEntitiesCount; i++) {
		destroyEntity(entities, destroyEntities[i], enitiesCount);
	}
}

void gameloop(SDL_Renderer* renderer) {
	SDL_Event event;
	const Uint8* keystate;
	int alienDirection = ALIEN_DIRECTION_RIGHT;
	int gameState = GAME_STATE_PLAY;

	int shipCount = 1;
	Entity* ship = createShip(renderer);

	int alienCount = -1;
	Entity* aliens = createAliens(renderer, &alienCount);

	int shipBulletCount = 0;
	Object* ship_bullets = (Object*)malloc(0 * sizeof(Object));

	int alienBulletCount = 0, alienBulletRelease = 3;
	Object* alien_bullets = (Object*)malloc(0 * sizeof(Object));

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

		if (accumulatedTicks >= TIME_STEP) {
			switch (gameState) {
			case GAME_STATE_PLAY:
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
						break;
					}
				}
				keystate = SDL_GetKeyboardState(NULL);
				if (ship)
					handleShipMovement(ship, keystate, &ship_bullets, &shipBulletCount, renderer, &lastShipFiringTime);
				moveAliens(aliens, alienCount, &alienDirection);

				// fire alien bullet every third render
				alienBulletRelease--;
				if (alienBulletRelease <= 0 && alienCount > 0) {
					fireBullet(render, aliens, &alien_bullets, &alienBulletCount, true, &alienCount);
					alienBulletRelease = 3;
				}

				moveBullets(alien_bullets, alienBulletCount, ship_bullets, shipBulletCount);
				evaluateCollisions(&ship_bullets, &shipBulletCount, &aliens, &alienCount);
				evaluateCollisions(&alien_bullets, &alienBulletCount, &ship, &(shipCount));

				render(renderer, ship, aliens, alienCount, ship_bullets, shipBulletCount, alien_bullets, alienBulletCount);
				accumulatedTicks -= TIME_STEP;
				
				break;
			case GAME_STATE_MENU:
				break;
			}
		}
		else {
			SDL_Delay(DELAY);
		}
	}
	// TODO: destroy all left-over textures
	if (ship) {
		SDL_DestroyTexture(ship->object.imageText);
	}
}

int main(int argc, char* args[])
{
	// Init window and PNG images loading
	SDL_Window* window = init();
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_ACCELERATED);

	gameloop(renderer);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}