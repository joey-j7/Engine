#include "pch.h"
#include "TransformComponent2D.h"

#include "TransformComponent3D.h"

namespace Engine
{
	TransformComponent2D::TransformComponent2D(Entity& Entity, const std::string& sName) : TransformComponent<Vector2, float>(Entity, sName)
	{
		AddProhibitedTypes<TransformComponent3D>();
	}
}
