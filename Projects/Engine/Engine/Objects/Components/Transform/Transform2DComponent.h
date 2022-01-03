#pragma once

#include "TransformComponent.h"

namespace Engine
{
	class Engine_API Transform2DComponent : public TransformComponent<Vector2, float>
	{
	public:
		Transform2DComponent(Entity& Entity, const String& sName = "Transform 2D Component");
	};
}
