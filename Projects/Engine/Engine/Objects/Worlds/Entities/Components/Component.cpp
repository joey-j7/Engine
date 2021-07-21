#include "pch.h"
#include "Component.h"
#include "Engine/Objects/Worlds/Entities/Entity.h"

namespace Engine
{
	Component::Component(Entity& Entity, const std::string& sName) : Object(sName), m_Entity(Entity)
	{

	}
}
