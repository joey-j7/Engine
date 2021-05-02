#pragma once

#include "Entity.h"
#include "Engine/Objects/Object.h"

namespace Engine
{
	class Engine_API StaticEntity : public Object, public Entity
	{
	public:
		friend class World;
		
		StaticEntity(const std::string& sName = "Unnamed Entity") : Object(sName) {}

	protected:
		virtual void Awake() override {};
	};
}