#pragma once

#include "Entity.h"
#include "Engine/Objects/DynamicObject.h"

namespace Engine
{
	class Engine_API DynamicEntity : public DynamicObject, public Entity
	{
	public:
		friend class World;
		
		DynamicEntity(const std::string& sName = "Unnamed Entity") : DynamicObject(sName), Entity()
		{
			m_Type = E_DYNAMIC;
		}

		virtual ~DynamicEntity()
		{
			OnDestroy();
		}

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