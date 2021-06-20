#include "pch.h"
#include "Entity.h"

namespace Engine
{
	Entity::~Entity()
	{
		SetParent(nullptr);
	}

	void Entity::SetParent(Entity* NewEntity)
	{
		if (Parent == NewEntity)
			return;

		if (Parent)
			Parent->Children.remove(this);

		Entity* OldEntity = Parent;
		Parent = NewEntity;
		
		OnParentChange(OldEntity, NewEntity);

		if (!Parent)
			return;

		Parent->Children.push_back(this);
	}
}