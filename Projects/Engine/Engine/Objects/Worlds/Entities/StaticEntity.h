#pragma once

#include "Entity.h"
#include "Engine/Objects/Object.h"

namespace Engine
{
	class Engine_API StaticEntity : public Object, public Entity
	{
	public:
		friend class World;
		
		StaticEntity(const String& sName = "Entity");
		const String& GetName() const override { return Object::GetName(); };
		
	protected:
		virtual void Awake() override {};
	};
}