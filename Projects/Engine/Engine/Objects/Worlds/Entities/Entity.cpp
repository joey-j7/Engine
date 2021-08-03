#include "pch.h"
#include "Entity.h"

#include "Components/RenderComponent.h"

namespace Engine
{
	Entity::~Entity()
	{
		// Pass along parent to children
		for (auto Child : m_Children)
		{
			Child->SetParent(GetParent());
		}
		
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
		uint32_t ID = Comp->GetID();
		
		auto Find = m_Components.find(ID);
		const bool Found = Find != m_Components.end();
		
		if (Found)
		{
			OnComponentRemoved(*this, *Find->second);
			CB_CORE_TRACE("Removed component {0} from {1}", Comp->GetName(), GetName());

			m_Components.erase(Find);
			
			// Remove all components that depend on this
			for (auto& Component : m_Components)
			{
				if (!Component.second->HasDependency(ID))
					continue;

				RemoveComponent(Component.second.get());
			}
		}
		
		return Found;
	}

	void Entity::SetParent(Entity* NewEntity)
	{
		if (m_Parent == NewEntity)
			return;
		
		if (m_Parent)
		{
			m_Parent->OnChildRemoved(*m_Parent, *this);
			m_Parent->m_Children.erase(std::find(m_Parent->m_Children.begin(), m_Parent->m_Children.end(), this));
		}

		Entity* OldEntity = m_Parent;
		m_Parent = NewEntity;
		
		OnParentChanged(*this, OldEntity, NewEntity);
		NotifyChildrenOnParentChanged(*this, OldEntity, NewEntity);

		CB_CORE_TRACE(
			"Parent for {0} changed from {1} to {2}",
			GetName(),
			OldEntity ? OldEntity->GetName() : "nullptr",
			NewEntity ? NewEntity->GetName() : "nullptr"
		);

		if (!m_Parent)
			return;

		m_Parent->m_Children.push_back(this);
		m_Parent->OnChildAdded(*m_Parent, *this);
	}

	void Entity::NotifyChildrenOnParentChanged(Entity& Origin, Entity* Old, Entity* New)
	{
		// Notify all children that this entity's parent has changed, nested loop
		for (Entity* Child : m_Children)
		{
			Child->OnParentChanged(Origin, Old, New);
			Child->NotifyChildrenOnParentChanged(Origin, Old, New);
		}
	}
}
