#pragma once

#include "TransformComponent.h"

namespace Engine
{
	class Engine_API Transform3DComponent : public TransformComponent<Vector3, Vector3>
	{
	public:
		Transform3DComponent(Entity& Entity, const String& sName = "Transform 3D Component");
	};
}
