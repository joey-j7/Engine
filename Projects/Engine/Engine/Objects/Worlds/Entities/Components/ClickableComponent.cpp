#include "pch.h"
#include "ClickableComponent.h"

#include "Transform/TransformComponent2D.h"
#include "Transform/TransformComponent3D.h"

#include "UI/Elements/UIText.h"

namespace Engine
{
	ClickableComponent::ClickableComponent(Entity& Entity, const std::string& sName) : Component(Entity, sName), Clickable()
	{
		if (!GetEntity().GetComponent<TransformComponent3D>())
		{
			AddDependencyTypes<TransformComponent2D>();
		}
	}

	const AABB ClickableComponent::GetBounds() const
	{
		return m_Entity.GetBounds();
	}
}
