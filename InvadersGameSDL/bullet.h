#pragma once

#include <SDL.h>
#include <stdbool.h>
#include "entity.h"

Object* createBullet(Entity* entity, SDL_Renderer* renderer, bool setAlienBullet, int* alienCount);

void fireBullet(SDL_Renderer* renderer, Entity* entity, Object** bullets, int* bulletCount, bool setAlienBullet, int* enitiesCount);

void moveBullets(Object* alienBullets, int alienBulletCount, Object* shipBullets, int shipBulletCount);

void destroyBullet(Object** bullets, Object* bullet, int* bulletCount);
