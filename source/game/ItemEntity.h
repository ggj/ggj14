#ifndef _ITEM_ENTITY_H
#define _ITEM_ENTITY_H

#include "SpriteEntity.h"

class ItemEntity: public SpriteEntity
{
	public:
		ItemEntity(const char *className, const char *spriteName);
};

#endif
