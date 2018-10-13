#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768
#define ENTITY_SIZE   30

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
	const char* imageName;
} Entity;

typedef struct {
	Entity entity;
	int health;
} Alien;

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

Alien* createAliens(SDL_Renderer* renderer) {
	Alien* aliens = malloc(4 * sizeof(Alien));

	// Create aliens
	for (int i = 0; i < 4; i++) {
		aliens[i].entity.imageRect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	}

	// Set names of images to aliens
	aliens[0].entity.imageName = GET_RESOURCE_PATH("alien_easy.png");
	aliens[1].entity.imageName = GET_RESOURCE_PATH("alien_medium.png");
	aliens[2].entity.imageName = GET_RESOURCE_PATH("alien_hard.png");
	aliens[3].entity.imageName = GET_RESOURCE_PATH("alien_boss.png");

	// Set surface, texture, position and size of aliens
	int offsetEntity = 2;
	for (int i = 0; i < 4; i++) {
		// Set surface and texture of alien image
		aliens[i].entity.imageSurf = IMG_Load(aliens[i].entity.imageName);
		aliens[i].entity.imageText = SDL_CreateTextureFromSurface(renderer, aliens[i].entity.imageSurf);

		// Set postion of each alien
		aliens[i].entity.imageRect->x = offsetEntity;
		aliens[i].entity.imageRect->y = 0;

		// Set size of each alien
		aliens[i].entity.imageRect->w = ENTITY_SIZE;
		aliens[i].entity.imageRect->h = ENTITY_SIZE;

		// Make small space between aliens => + 2
		offsetEntity += ENTITY_SIZE + 2;
	}

	return aliens;
}

Entity* createShip(SDL_Renderer* renderer) {
	// Create ship
	Entity* ship = (Entity*)malloc(sizeof(Entity));

	// Set image name
	ship->imageName = GET_RESOURCE_PATH("shipInvaders.png");

	// Set image surface and texture
	ship->imageSurf = IMG_Load(ship->imageName);
	ship->imageText = SDL_CreateTextureFromSurface(renderer, ship->imageSurf);

	// Set ship position and size
	ship->imageRect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	ship->imageRect->x = SCREEN_WIDTH / 2;
	ship->imageRect->y = SCREEN_HEIGHT - (ENTITY_SIZE*2);
	ship->imageRect->w = ENTITY_SIZE;
	ship->imageRect->h = ENTITY_SIZE;

	return ship;
}

int moveAliens(Alien* aliens, int elapsedTicks, int* direction) {
	bool edgeCollision = false;
	for (int i = 0; i < ALIEN_COUNT; i++) {
		SDL_Rect* alienPos = aliens[i].entity.imageRect;
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
		for (int i = 0; i < ALIEN_COUNT; i++) {
			SDL_Rect* alienPos = aliens[i].entity.imageRect;
			alienPos->y += ALIEN_VERTICAL_SPEED;
		}
	}
	else {
		int moveBy = ALIEN_HORIZONTAL_SPEED * elapsedTicks * (*direction);
		for (int i = 0; i < ALIEN_COUNT; i++) {
			SDL_Rect* alienPos = aliens[i].entity.imageRect;
			alienPos->x += moveBy;
		}
	}

	return 0;
}

void handleShipMovement(const Uint8 *keystate, int elapsedTicks, Entity* ship) {
	int moveBy = PLAYER_SPEED * elapsedTicks;
	if (keystate[SDL_SCANCODE_LEFT]) {
		ship->imageRect->x -= moveBy;
	}
	if (keystate[SDL_SCANCODE_RIGHT]) {
		ship->imageRect->x += moveBy;
	}
	if (keystate[SDL_SCANCODE_UP]) {
		//ship->imageRect->y += moveBy;
	}
	if (keystate[SDL_SCANCODE_DOWN]) {
		//ship->imageRect->y -= moveBy;
	}
	if (keystate[SDL_SCANCODE_SPACE]) {
		// instantiate a bullet and add to array ?
	}

	// Collide with edges of screen
	if (ship->imageRect->x < 0) {
		ship->imageRect->x = 0;
	}
	else if (ship->imageRect->x > SCREEN_WIDTH - ENTITY_SIZE) {
		ship->imageRect->x = SCREEN_WIDTH - ENTITY_SIZE;
	}
	if (ship->imageRect->y < 0) {
		ship->imageRect->y = 0;
	}
	else if (ship->imageRect->y > SCREEN_HEIGHT - ENTITY_SIZE) {
		ship->imageRect->y = SCREEN_HEIGHT - ENTITY_SIZE;
	}
}

void render(SDL_Renderer* renderer, Entity* ship, Alien* aliens) {
	SDL_RenderClear(renderer);

	// Set background color to renderer, copy to renderer ship texture with new postion and present renderer
	SDL_SetRenderDrawColor(renderer, R, G, B, 255);
	SDL_RenderCopy(renderer, ship->imageText, NULL, ship->imageRect);
	for (int i = 0; i < 4; i++)
		SDL_RenderCopy(renderer, aliens[i].entity.imageText, NULL, aliens[i].entity.imageRect);

	SDL_RenderPresent(renderer);
}

void gameloop(SDL_Renderer* renderer, Entity* ship, Alien* aliens) {
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
				handleShipMovement(keystate, elapsedTicks, ship);
				moveAliens(aliens, elapsedTicks, &alienDirection);

				render(renderer, ship, aliens);
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

	Entity* ship = createShip(renderer);

	Alien* aliens = createAliens(renderer);

	gameloop(renderer, ship, aliens);

	// TODO: destroy all textures
	SDL_DestroyTexture(ship->imageText);
	SDL_FreeSurface(screen);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}