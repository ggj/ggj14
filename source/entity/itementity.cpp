#include "itementity.h"
#include "entityfactory.h"
#include "../scene/gamescene.h"
#include "../util/sounds.h"

ItemEntity::ItemEntity(const char *className, const char *spriteName)
	: SpriteEntity(className, spriteName)
{
}

ItemEntity::~ItemEntity()
{
}

void ItemEntity::Load(MetadataObject &metadata, SceneNode *sprites)
{
	SpriteEntity::Load(metadata, sprites);
	pSprite->SetZ(-10);
	clSensor.Load(metadata, this);
}

void ItemEntity::Update(f32 dt)
{
	UNUSED(dt);
}

void ItemEntity::OnCollision(const CollisionEvent &event)
{
	UNUSED(event);
}

