#include "pch.h"
#include "Entity.h"

namespace Engine
{
	Entity::~Entity()
	{
		SetParent(nullptr);
	}

	const AABB Entity::GetBounds() const
	{
		AABB Bounds;
		
		Bounds.lowerBound = Vector2(FLT_MAX);
		Bounds.upperBound = Vector2(FLT_MIN);
		
		for (auto& Component : m_RenderComponents)
		{
			const AABB Bounds2 = Component->GetBounds();

			Bounds.lowerBound.x = glm::min(Bounds.lowerBound.x, Bounds2.lowerBound.x);
			Bounds.lowerBound.y = glm::min(Bounds.lowerBound.y, Bounds2.lowerBound.y);
			Bounds.upperBound.x = glm::max(Bounds.upperBound.x, Bounds2.upperBound.x);
			Bounds.upperBound.y = glm::max(Bounds.upperBound.y, Bounds2.upperBound.y);
		}

		if (Bounds.lowerBound == Vector2(FLT_MAX) || Bounds.upperBound == Vector2(FLT_MIN))
		{
			Bounds.lowerBound = Vector2(0.f);
			Bounds.upperBound = Vector2(0.f);
		}
		
		return Bounds;
	}

	bool Entity::RemoveComponent(Component* Comp)
	{
		auto Find = m_Components.find(Comp->GetID());
		const bool Found = Find != m_Components.end();
		
		if (Found)
		{
			OnComponentRemoved(*Find->second);
			m_Components.erase(Find);
		}
		
		return Found;
	}

	void Entity::SetParent(Entity* NewEntity)
	{
		if (Parent == NewEntity)
			return;

		if (Parent)
			Parent->Children.remove(this);

		Entity* OldEntity = Parent;
		Parent = NewEntity;
		
		OnParentChanged(OldEntity, NewEntity);

		if (!Parent)
			return;

		Parent->Children.push_back(this);
	}
}
