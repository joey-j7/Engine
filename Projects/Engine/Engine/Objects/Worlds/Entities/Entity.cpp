#include "pch.h"
#include "Entity.h"

#include "Components/RenderComponent.h"

namespace Engine
{
	Entity::~Entity()
	{
		SetParent(nullptr);
	}

	const AABB Entity::GetBounds() const
	{
		AABB Bounds;
		
		Bounds.fLeft = FLT_MAX;
		Bounds.fTop = FLT_MAX;
		Bounds.fRight = FLT_MIN;
		Bounds.fBottom = FLT_MIN;

		auto Render3DComponents = GetComponentsOfType<Render3DComponent>();
		
		for (auto& Component : Render3DComponents)
		{
			const AABB Bounds2 = Component->GetBounds();

			Bounds.fLeft = glm::min(Bounds.fLeft, Bounds2.fLeft);
			Bounds.fTop = glm::min(Bounds.fTop, Bounds2.fTop);
			Bounds.fRight = glm::max(Bounds.fRight, Bounds2.fRight);
			Bounds.fBottom = glm::max(Bounds.fBottom, Bounds2.fBottom);
		}

		auto Render2DComponents = GetComponentsOfType<Render2DComponent>();

		for (auto& Component : Render2DComponents)
		{
			const AABB Bounds2 = Component->GetBounds();

			Bounds.fLeft = glm::min(Bounds.fLeft, Bounds2.fLeft);
			Bounds.fTop = glm::min(Bounds.fTop, Bounds2.fTop);
			Bounds.fRight = glm::max(Bounds.fRight, Bounds2.fRight);
			Bounds.fBottom = glm::max(Bounds.fBottom, Bounds2.fBottom);
		}

		if (Bounds.fLeft == FLT_MAX || Bounds.fTop == FLT_MAX || Bounds.fRight == FLT_MIN || Bounds.fBottom == FLT_MIN)
		{
			Bounds.fLeft = Bounds.fTop = Bounds.fRight = Bounds.fBottom = 0.f;
		}
		
		return Bounds;
	}

	bool Entity::RemoveComponent(Component* Comp)
	{
		auto Find = m_Components.find(Comp->GetID());
		const bool Found = Find != m_Components.end();
		
		if (Found)
		{
			OnComponentRemoved(*this, *Find->second);
			m_Components.erase(Find);
		}
		
		return Found;
	}

	void Entity::SetParent(Entity* NewEntity)
	{
		if (Parent == NewEntity)
			return;

		if (Parent)
		{
			Parent->OnChildRemoved(*Parent, *this);
			Parent->Children.remove(this);
		}

		Entity* OldEntity = Parent;
		Parent = NewEntity;
		
		OnParentChanged(*this, OldEntity, NewEntity);

		if (!Parent)
			return;

		Parent->Children.push_back(this);
		Parent->OnChildAdded(*Parent, *this);
	}
}
