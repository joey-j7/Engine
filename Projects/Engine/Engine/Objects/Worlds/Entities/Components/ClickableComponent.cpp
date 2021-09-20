#include "pch.h"
#include "ClickableComponent.h"

#include "Transform/Transform2DComponent.h"
#include "Transform/Transform3DComponent.h"

namespace Engine
{
	ClickableComponent::ClickableComponent(Entity& Entity, const String& sName) : Component(Entity, sName), Clickable()
	{
		if (!GetEntity().GetComponent<Transform3DComponent>())
		{
			AddDependencyTypes<Transform2DComponent>();
		}
	}

	const AABB ClickableComponent::GetBounds() const
	{
		return m_Entity.GetBounds();
	}
}
