#pragma once

#include "Engine/Objects/Object.h"
#include "Engine/Objects/Worlds/Entities/Entity.h"

namespace Engine
{
	class Engine_API Component : public Object
	{
	public:
		Component(Entity& Entity, const std::string& sName = "Unnamed Component") : Object(sName), m_Entity(Entity) {};

		Entity& GetEntity() const { return m_Entity; }

		bool HasForcedUniqueness() const { return m_ForceUniqueness; }

		virtual const std::list<std::type_info> GetDependencyComponents() const { return {}; };
		virtual const std::list<std::type_info> GetProhibitedComponents() const { return {}; };
		
	protected:
		Entity& m_Entity;
		bool m_ForceUniqueness = false;
	};
}
