#include "pch.h"

#include "Entity.h"
#include "Components/Component.h"

namespace Engine
{
	Entity::~Entity()
	{
		SetParent(nullptr);

		for (auto& Component : m_Components)
		{
			delete Component.second;
		}
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
