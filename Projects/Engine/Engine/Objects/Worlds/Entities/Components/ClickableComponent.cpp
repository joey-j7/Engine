#include "pch.h"
#include "ClickableComponent.h"

#include "Transform/Transform2DComponent.h"
#include "Transform/Transform3DComponent.h"

#include "Engine/Objects/Worlds/World.h"

namespace Engine
{
	ClickableComponent::ClickableComponent(Entity& Entity, const String& sName) : Component(Entity, sName), Clickable()
	{
		if (!GetEntity().GetComponent<Transform3DComponent>())
		{
			AddDependencyTypes<Transform2DComponent>();
		}

		m_VisibilityHandler = GetEntity().OnVisibilityChanged.Bind(this, &ClickableComponent::OnVisibilityChanged);
	}

	ClickableComponent::~ClickableComponent()
	{
		GetEntity().OnVisibilityChanged.Unbind(m_VisibilityHandler);
	}

	const AABB ClickableComponent::GetBounds() const
	{
		return m_Entity.GetBounds();
	}

	bool ClickableComponent::IsClickable() const
	{
		return Clickable::IsClickable() && GetEntity().GetWorld()->IsActive();
	}

	void ClickableComponent::OnVisibilityChanged(Entity::Visibility Vis)
	{
		m_IsActive = Vis != Entity::E_COLLAPSED;

		const DVector2& Position = m_Window.GetMousePosition();

		const AABB Bounds = GetBounds();
		const bool Hover = CanFocus() && Bounds.contains(Position.x, Position.y);

		if (m_IsActive)
		{
			OnCursorPosition(Position, DVector2(0.0));
		}
		else
		{
			if (m_IsPressed)
				OnReleased();

			if (Hover)
				OnExit(Position);
		}
	}
}
