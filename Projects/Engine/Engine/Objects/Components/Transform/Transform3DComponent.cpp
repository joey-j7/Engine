#include "pch.h"
#include "Transform3DComponent.h"

#include "Transform2DComponent.h"

namespace Engine
{
	Transform3DComponent::Transform3DComponent(Entity& Entity, const String& sName) : TransformComponent<Vector3, Vector3>(Entity, sName)
	{
		AddProhibitedTypes<Transform2DComponent>();
	}
}
