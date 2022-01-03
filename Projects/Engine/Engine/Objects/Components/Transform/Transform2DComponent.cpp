#include "pch.h"
#include "Transform2DComponent.h"

#include "Transform3DComponent.h"

namespace Engine
{
	Transform2DComponent::Transform2DComponent(Entity& Entity, const String& sName) : TransformComponent<Vector2, float>(Entity, sName)
	{
		AddProhibitedTypes<Transform3DComponent>();
	}
}
