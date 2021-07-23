#pragma once

#include "TransformComponent.h"

namespace Engine
{
	class Engine_API TransformComponent3D : public TransformComponent<Vector3, Vector3>
	{
	public:
		TransformComponent3D(Entity& Entity, const std::string& sName = "Transform 3D Component");
	};

	Engine_REGISTER_COMP(TransformComponent3D);
}
