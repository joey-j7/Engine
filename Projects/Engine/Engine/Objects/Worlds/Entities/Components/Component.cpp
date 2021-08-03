#include "pch.h"
#include "Component.h"

namespace Engine
{
	Component::Component(Entity& Entity, const String& sName) : Object(sName), m_Entity(Entity)
	{

	}
}
