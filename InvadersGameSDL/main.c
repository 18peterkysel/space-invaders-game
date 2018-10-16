#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768
#define ENTITY_SIZE   30
#define BULLET_SIZE_WIDTH 7
#define BULLET_SIZE_HEIGHT 20

#define TIME_STEP 1000/30
#define DELAY 5

#define GAME_STATE_MENU 1
#define GAME_STATE_PLAY 2
#define GAME_STATE_PAUSE 3
#define GAME_STATE_QUIT 0

#define ALIEN_DIRECTION_RIGHT 1
#define ALIEN_DIRECTION_LEFT -1
#define ALIEN_HORIZONTAL_SPEED 1
#define ALIEN_VERTICAL_SPEED 40
#define ALIEN_HORIZONTAL_COUNT 4
#define ALIEN_VERTICAL_COUNT 1
#define ALIEN_COUNT (ALIEN_HORIZONTAL_COUNT * ALIEN_VERTICAL_COUNT)

#define PLAYER_SPEED 3

#define GET_RESOURCE_PATH(res_name) "./resources/"res_name

// Define background color in R, G, B
#define R 0
#define G 0
#define B 0

// Creation of structs
typedef struct {
	SDL_Surface* imageSurf;
	SDL_Texture* imageText;
	SDL_Rect* imageRect;
	char* imageName;
} Object;

typedef struct {
	Object* object;
	int* health;
} Entity;

// Define arrays
Entity* ship;
Object* bullets;
Entity* aliens;

int bulletsCount = 0;
int aliensCount = 4;

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

void createAliens(SDL_Renderer* renderer) {
	aliens = malloc(aliensCount * sizeof(Entity));

	// Create aliens
	for (int i = 0; i < 4; i++) {
		aliens[i].object = (Object*)malloc(sizeof(Object));
		aliens[i].object->imageRect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
		aliens[i].health = 50;
	}

	// Set names of images to aliens
	aliens[0].object->imageName = GET_RESOURCE_PATH("alien_easy.png");
	aliens[1].object->imageName = GET_RESOURCE_PATH("alien_medium.png");
	aliens[2].object->imageName = GET_RESOURCE_PATH("alien_hard.png");
	aliens[3].object->imageName = GET_RESOURCE_PATH("alien_boss.png");

	// Set surface, texture, position and size of aliens
	int offsetEntity = 2;
	for (int i = 0; i < 4; i++) {
		// Set surface and texture of alien image
		aliens[i].object->imageSurf = IMG_Load(aliens[i].object->imageName);
		aliens[i].object->imageText = SDL_CreateTextureFromSurface(renderer, aliens[i].object->imageSurf);

		// Set postion of each alien
		aliens[i].object->imageRect->x = offsetEntity;
		aliens[i].object->imageRect->y = 0;

		// Set size of each alien
		aliens[i].object->imageRect->w = ENTITY_SIZE;
		aliens[i].object->imageRect->h = ENTITY_SIZE;

		// Make small space between aliens => + 2
		offsetEntity += ENTITY_SIZE + 2;
	}
}

void createShip(SDL_Renderer* renderer) {
	// Create ship
	ship = (Entity*)malloc(sizeof(Entity));
	ship->object = (Object*)malloc(sizeof(Object));
	ship->health = 50;

	// Set image name
	ship->object->imageName = GET_RESOURCE_PATH("shipInvaders.png");

	// Set image surface and texture
	ship->object->imageSurf = IMG_Load(ship->object->imageName);
	ship->object->imageText = SDL_CreateTextureFromSurface(renderer, ship->object->imageSurf);

	// Set ship position and size
	ship->object->imageRect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	ship->object->imageRect->x = SCREEN_WIDTH / 2;
	ship->object->imageRect->y = SCREEN_HEIGHT - (ENTITY_SIZE*2);
	ship->object->imageRect->w = ENTITY_SIZE;
	ship->object->imageRect->h = ENTITY_SIZE;
}

Object* createBullet(Entity* ship, SDL_Renderer* renderer) {
	// Create bullet
	Object* bullet = (Object*)malloc(sizeof(Object));

	// Set image name
	bullet->imageName = GET_RESOURCE_PATH("bullet.png");

	// Set image surface and texture
	bullet->imageSurf = IMG_Load(bullet->imageName);
	bullet->imageText = SDL_CreateTextureFromSurface(renderer, bullet->imageSurf);

	// Set bullet position and size
	bullet->imageRect = (SDL_Rect*)malloc(sizeof(SDL_Rect));

	int x_ship = ship->object->imageRect->x;
	int y_ship = ship->object->imageRect->y;

	bullet->imageRect->x = x_ship + (ENTITY_SIZE / 2) - (BULLET_SIZE_WIDTH / 2);
	bullet->imageRect->y = y_ship;
	bullet->imageRect->w = BULLET_SIZE_WIDTH;
	bullet->imageRect->h = BULLET_SIZE_HEIGHT;

	return bullet;
}

void fireBullet(SDL_Renderer* renderer) {
	// Realloc array of bullets
	bullets = (Object*)realloc(bullets, (bulletsCount + 1)*sizeof(Object));

	// Add bullet to array of bullets
	Object* bullet = createBullet(ship, renderer);
	bullets[bulletsCount] = *bullet;
	bulletsCount++;
}

int moveAliens(int elapsedTicks, int* direction) {
	bool edgeCollision = false;
	for (int i = 0; i < aliensCount; i++) {
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
		for (int i = 0; i < aliensCount; i++) {
			SDL_Rect* alienPos = aliens[i].object->imageRect;
			alienPos->y += ALIEN_VERTICAL_SPEED;
		}
	}
	else {
		int moveBy = ALIEN_HORIZONTAL_SPEED * elapsedTicks * (*direction);
		for (int i = 0; i < aliensCount; i++) {
			SDL_Rect* alienPos = aliens[i].object->imageRect;
			alienPos->x += moveBy;
		}
	}

	return 0;
}

int moveBullets() {
	if (bulletsCount == 0) {
		return -1;
	}

	for (int i = 0; i < bulletsCount; i++) {
		bullets[i].imageRect->y--;
	}

	return 0;
}

void handleShipMovement(SDL_Renderer* renderer, const Uint8 *keystate, int elapsedTicks) {
	int moveBy = PLAYER_SPEED * elapsedTicks;
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
		// instantiate a bullet and add to array ?
		fireBullet(renderer);
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

void render(SDL_Renderer* renderer) {
	SDL_RenderClear(renderer);

	// Set background color to renderer, copy to renderer ship texture with new postion and present renderer
	SDL_SetRenderDrawColor(renderer, R, G, B, 255);
	SDL_RenderCopy(renderer, ship->object->imageText, NULL, ship->object->imageRect);
	for (int i = 0; i < aliensCount; i++)
		SDL_RenderCopy(renderer, aliens[i].object->imageText, NULL, aliens[i].object->imageRect);

	for (int i = 0; i < bulletsCount; i++)
		SDL_RenderCopy(renderer, bullets[i].imageText, NULL, bullets[i].imageRect);

	SDL_RenderPresent(renderer);
}

void destroyBullet(Object* bullet) {
	// Get postion of bullet to detroy
	int position = -1;
	for (int i = 0; i < bulletsCount; i++) {
		if (&(bullets[i]) == bullet) {
			position = i;
			break;
		}
	}

	if (position == -1) {
		printf("ERROR(destroyBullet): Cannot find address of bullet in array!\n");
		return;
	}

	// Remove bullet from bullets array
	if (position != (bulletsCount - 1)) {
		for (position; position < bulletsCount - 1; position++) {
			bullets[position] = bullets[position + 1];
		}
	}

	bulletsCount--;
	bullets = (Object*)realloc(bullets, bulletsCount * sizeof(Object));

	// TODO: Detroy resources and free memmory
//	SDL_DestroyTexture(bullet->imageText);
//	SDL_FreeSurface(bullet->imageSurf);
//	free(bullet->imageRect);

//	free(bullet->imageName);
//	free(&bullet);
}

void destroyAlien(Entity* alien) {
	// Get postion of alien to detroy
	int position = -1;
	for (int i = 0; i < aliensCount; i++) {
		if (&(aliens[i]) == alien) {
			position = i;
			break;
		}
	}

	if (position == -1) {
		printf("ERROR(destroyAlien): Cannot find address of alien in array!\n");
		return;
	}

	// Remove alien from aliens array
	if (position != (aliensCount - 1)) {
		for (position; position < aliensCount - 1; position++) {
			aliens[position] = aliens[position + 1];
		}
	}

	aliensCount--;
	aliens = (Entity*)realloc(aliens, aliensCount * sizeof(Entity));

	// TODO: Detroy resources and free memmory
//	SDL_DestroyTexture(alien->object->imageText);
//	SDL_FreeSurface(alien->object->imageSurf);

//	free(alien->object->imageName);
//	free(alien->object->imageRect);
//	free(alien->health);
//	free(alien->object);
//	free(&alien);
}

void evaluateCollisions() {
	int destBulletsCount = 0;
	int destAliensCount = 0;
	Object** destBullets = (Object**)malloc(sizeof(Object));;
	Entity** destAliens = (Entity**)malloc(sizeof(Entity));;

	for (int i = 0; i < bulletsCount; i++) {
		for (int j = 0; j < aliensCount; j++) {
			if (SDL_HasIntersection(bullets[i].imageRect, aliens[j].object->imageRect)) {
				// TODO: Check if alien is already add to destAliens array
				destAliens = (Entity**)realloc(destAliens, (destAliensCount + 1) * sizeof(Entity));
				destAliens[destAliensCount] = &(aliens[j]);
				destAliensCount++;

				destBullets = (Object**)realloc(destBullets, (destBulletsCount + 1) * sizeof(Object));
				destBullets[destBulletsCount] = &(bullets[i]);
				destBulletsCount++;
				break;
			}
		}
		if (bullets[i].imageRect->y <= 0) {
			destBullets = (Object**)realloc(destBullets, (destBulletsCount + 1) * sizeof(Object));
			destBullets[destBulletsCount] = &(bullets[i]);
			destBulletsCount++;
		}
	}

	if (destBulletsCount != 0) {
		for (int i = 0; i < destBulletsCount; i++) {
			destroyBullet(destBullets[i]);
		}
		if (destAliensCount != 0) {
			for (int i = 0; i < destAliensCount; i++) {
				destroyAlien(destAliens[i]);
			}
		}
	}
}

void gameloop(SDL_Renderer* renderer) {
	SDL_Event event;
	Uint8* keystate;
	int alienDirection = ALIEN_DIRECTION_RIGHT;
	int gameState = GAME_STATE_PLAY;

	// variable time-step
	int currentTicks = -1, elapsedTicks = -1, accumulatedTicks = 0, lastTicks = SDL_GetTicks();
	while (gameState != GAME_STATE_QUIT) {
		currentTicks = SDL_GetTicks();
		elapsedTicks = currentTicks - lastTicks;
		accumulatedTicks += elapsedTicks;
		lastTicks = currentTicks;
		if (accumulatedTicks >= TIME_STEP) {
			switch (gameState) {
			case GAME_STATE_PLAY:
				// processInput
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
					}
				}
				keystate = SDL_GetKeyboardState(NULL);
				handleShipMovement(renderer, keystate, elapsedTicks);
				moveAliens(elapsedTicks, &alienDirection);
				moveBullets();
				evaluateCollisions();

				render(renderer);
				break;
			case GAME_STATE_MENU:
				break;
			}
			accumulatedTicks -= TIME_STEP;
		}
		else {
			SDL_Delay(DELAY);
		}
	}
}

int main(int argc, char* args[])
{
	// Init window and PNG images loading
	SDL_Window* window = init();
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);
	SDL_Surface* screen = SDL_GetWindowSurface(window);

	createShip(renderer);
	createAliens(renderer);

	gameloop(renderer);

	// TODO: destroy all textures
	SDL_DestroyTexture(ship->object->imageText);
	SDL_FreeSurface(screen);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}