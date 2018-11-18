#include "entity.h"

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

	SDL_DestroyTexture(entity->object->imageText);
	free(entity->object->imageRect);
	free(entity->object);

	// Remove entity from entities array
	if (position != (*entitiesCount - 1)) {
		for (; position < *entitiesCount - 1; position++) {
			(*entities)[position] = (*entities)[position + 1];
		}
	}

	(*entitiesCount)--;
	(*entities) = (Entity*)realloc(*entities, *entitiesCount * sizeof(Entity));
}
