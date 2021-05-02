#pragma once

#include <vector>

#include "Entities/Entity.h"
#include "Entities/DynamicEntity.h"

#include "Engine/Managers/IDManager.h"
#include "Engine/Objects/LayeredObject.h"

namespace Engine
{
	class Engine_API World : public LayeredObject
	{
		friend class WorldManagerLayer;

	public:
		World(const std::string& sName = "Unnamed World");
		virtual ~World();

		void Add(Entity* pObject);
		bool Remove(Entity* pObject);

	protected:
		virtual void Update(float fDeltaTime) override;
		virtual void FixedUpdate(float fDeltaTime) override;

		virtual void Draw(float fDeltaTime) override;
		virtual void LateUpdate(float fDeltaTime) override;

		virtual bool Pause() override;
		virtual bool Resume() override;
		virtual bool Stop() override;

	private:
		IDManager m_IDManager;

		std::vector<Entity*> m_pWorldObjects;
		
		std::vector<DynamicEntity*> m_pDynamicWorldObjects;
		std::vector<DynamicEntity*> m_pObjectsToActivate;

		std::vector<Entity*> m_pObjectsToRemove;
	};
}
