#pragma once

#include "EntityComponent.h"
#include "Engine/Objects/DynamicObject.h"

namespace Engine
{
	class Engine_API DynamicEntity : public DynamicObject, public Entity
	{
	public:
		friend class World;
		
		DynamicEntity(const String& sName = "Entity");
		virtual ~DynamicEntity();

		virtual const String& GetName() const override { return DynamicObject::GetName(); };
		
	protected:
		virtual void Awake() override {};
		
		virtual void Update(float fDeltaTime) override {};
		virtual void FixedUpdate(float fDeltaTime) override {};

		virtual void Draw(float fDeltaTime) override {};
		virtual void LateUpdate(float fDeltaTime) override {};

		virtual bool OnDestroy() override
		{
			Stop();
			return true;
		}
	};
}