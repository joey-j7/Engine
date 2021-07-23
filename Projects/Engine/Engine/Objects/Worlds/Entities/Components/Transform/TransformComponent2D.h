#pragma once

#include "TransformComponent.h"

namespace Engine
{
	class Engine_API TransformComponent2D : public TransformComponent<Vector2, float>
	{
	public:
		TransformComponent2D(Entity& Entity, const std::string& sName = "Transform 2D Component");
	};
	
	Engine_REGISTER_COMP(TransformComponent2D);
}
