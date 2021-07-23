#include "pch.h"
#include "TransformComponent3D.h"

#include "TransformComponent2D.h"

namespace Engine
{
	TransformComponent3D::TransformComponent3D(Entity& Entity, const std::string& sName) : TransformComponent<Vector3, Vector3>(Entity, sName)
	{
		AddProhibitedTypes<TransformComponent2D>();
	}
}
