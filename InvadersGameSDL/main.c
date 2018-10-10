#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// It doesn't work if array is allocated dynamically: char *dyn_arr=new char[100]
#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 768
#define ENTITY_SIZE   20

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
}Entity;

typedef struct {
	Entity* entity;
	int health;
}Alien;

// Support functions
void init() 
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		exit;
	}

	//Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		exit;
	}

}

Alien* createAliens(SDL_Renderer* renderer) {
	Alien aliens[4];
	
	// Create aliens
	for (int i = 0; i < 4; i++) {
		aliens[i].entity = (Entity*)malloc(sizeof(Entity));
		aliens[i].entity->imageRect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	}

	/*Alien *alienEasy = (Alien*)malloc(sizeof(Alien));
	Alien *alienMedium = (Alien*)malloc(sizeof(Alien));
	Alien *alienHard = (Alien*)malloc(sizeof(Alien));
	Alien *alienBoss = (Alien*)malloc(sizeof(Alien));*/

	// Fill in an array of aliens
	/*aliens[0] = *alienEasy;
	aliens[1] = *alienMedium;
	aliens[2] = *alienHard;
	aliens[3] = *alienBoss;*/

	// Inicialization of entity
	/*for (int i = 0; i < 4; i++)
		aliens[i].entity = (Entity*)malloc(sizeof(Entity));*/

	// Set names of images to aliens
	aliens[0].entity->imageName = "alien_easy.png";
	aliens[1].entity->imageName = "alien_medium.png";
	aliens[2].entity->imageName = "alien_hard.png";
	aliens[3].entity->imageName = "alien_boss.png";

	// Set surface, texture, position and size of aliens
	int offSetEntity = 2;
	for (int i = 0; i < 4; i++) {
		// Set surface and texture of alien image
		aliens[i].entity->imageSurf = IMG_Load(aliens[i].entity->imageName);
		aliens[i].entity->imageText = SDL_CreateTextureFromSurface(renderer, aliens[i].entity->imageSurf);

		// Set postion of each alien
		aliens[i].entity->imageRect->x = offSetEntity;
		aliens[i].entity->imageRect->y = 0;

		// Set size of each alien
		aliens[i].entity->imageRect->w = ENTITY_SIZE;
		aliens[i].entity->imageRect->h = ENTITY_SIZE;

		// Make small space between aliens => + 2
		offSetEntity += ENTITY_SIZE + 2;
	}
	
	return aliens;
}

int setAliensPositions(Alien* aliens, SDL_Renderer* renderer, bool right, bool left, bool down) {
	for (int i = 0; i < 4; i++) {
		// If move to down, move by 1 line
		if (down) 
		{
			aliens[i].entity->imageRect->y += 1;
			continue;
		}

		// If move to right, move by 5, otherwise, move to left by 5
		if (right) 
		{
			if (aliens[i].entity->imageRect->x + 5 > SCREEN_WIDTH - ENTITY_SIZE) {
				return -1;
			}
			aliens[i].entity->imageRect->x += 5;
		}
		else 
		{
			if (aliens[i].entity->imageRect->x - 5 > 0) {
				return -1;
			}
			aliens[i].entity->imageRect->x -= 5;
		}
	}

	return 0;
}

void handleShipMovement(const Uint8 *keystate, Entity* ship) {
	if (keystate[SDL_SCANCODE_LEFT]) {
		ship->imageRect->x -= 1;
	}
	if (keystate[SDL_SCANCODE_RIGHT]) {
		ship->imageRect->x += 1;
	}
	if (keystate[SDL_SCANCODE_UP]) {
		ship->imageRect->y -= 1;
	}
	if (keystate[SDL_SCANCODE_DOWN]) {
		ship->imageRect->y += 1;
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

void rendScreen(SDL_Renderer* renderer, Entity* ship, Alien* aliens) {
	/*Alien tmp = aliens[0];
	Alien tmp1 = aliens[1];
	Alien tmp2 = aliens[2];
	Alien tmp3 = aliens[3];*/

	// Clear renderer
	SDL_RenderClear(renderer);

	// Set background color to renderer, copy to renderer ship texture with new postion and present renderer
	SDL_SetRenderDrawColor(renderer, R, G, B, 255);
	SDL_RenderCopy(renderer, ship->imageText, NULL, ship->imageRect);
	for (int i = 0; i < 4; i++)
		SDL_RenderCopy(renderer, aliens[i].entity->imageText, NULL, aliens[i].entity->imageRect);

	SDL_RenderPresent(renderer);
}

int main(int argc, char* args[])
{	
	// Init window and PNG images loading
	init();

	//Create window
	SDL_Window* window;
	window = SDL_CreateWindow("Invaders Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

	if (window == NULL) {
		printf("Window cannot be NULL !");
		exit;
	}
	
	// Create the main renderer
	SDL_Renderer* mainRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);

	// Get window surface
	SDL_Surface* screen = SDL_GetWindowSurface(window);
	
	// Create ship
	Entity* ship = (Entity*)malloc(sizeof(Entity));

	// Set image name
	ship->imageName = "shipInvaders.png";

	// Set image surface and texture
	ship->imageSurf = IMG_Load(ship->imageName);
	ship->imageText = SDL_CreateTextureFromSurface(mainRenderer, ship->imageSurf);

	// Set ship position and size
	ship->imageRect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
	ship->imageRect->x = SCREEN_WIDTH / 2;
	ship->imageRect->y = SCREEN_HEIGHT - ENTITY_SIZE;
	ship->imageRect->w = ENTITY_SIZE;
	ship->imageRect->h = ENTITY_SIZE;

	// Create and set aliens on screen
	Alien* aliens = createAliens(mainRenderer);

	Alien tmp = aliens[0];
	Alien tmp1 = aliens[1];
	Alien tmp2 = aliens[2];
	Alien tmp3 = aliens[3];

	// Rend screen
	rendScreen(mainRenderer, ship, aliens);

	// Wait
	SDL_Delay(2000);

	SDL_Event event;

	bool moveRight = true, moveLeft = false;
	const Uint8 *keystate;
	int gameover = 0;

	// Message pump
	while (!gameover)
	{
		// Look for an event
		if (SDL_PollEvent(&event)) {
			// An event was found
			switch (event.type) {
				// Close button clicked
			case SDL_QUIT:
				gameover = 1;
				break;

				// Handle the keyboard
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
				case SDLK_q:
					gameover = 1;
					break;
				}
				break;
			}
		}

		// Handle ship movement
		keystate = SDL_GetKeyboardState(NULL);
		handleShipMovement(keystate, ship);

		// Move aliens
		int collapsed;
		if (moveRight) {
			collapsed = setAliensPositions(aliens, mainRenderer, true, false, false);
		}
		else
		{
			collapsed = setAliensPositions(aliens, mainRenderer, false, true, false);
		}

		if (collapsed == -1) {
			setAliensPositions(aliens, mainRenderer, false, false, true);
			moveRight = !moveRight;
			moveLeft = !moveLeft;
		}

		// Clear renderer
		SDL_RenderClear(mainRenderer);

		// Set background color to renderer, copy to renderer ship texture with new postion and present renderer
		SDL_SetRenderDrawColor(mainRenderer, R, G, B, 255);
		SDL_RenderCopy(mainRenderer, ship->imageText, NULL, ship->imageRect);
		SDL_RenderPresent(mainRenderer);
	}

	// Destroy texture
	SDL_DestroyTexture(ship->imageText);

	// Destroy surface
	SDL_FreeSurface(screen);

	// Destroy window
	SDL_DestroyWindow(window);

	// Quit game
	SDL_Quit();

	return 0;
}