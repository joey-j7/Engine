#pragma once

#include <vector>

#include "Engine/Objects/Entities/EntityComponent.h"

#include "Engine/Managers/IDManager.h"
#include "Engine/Objects/Layers/Layerable.h"

namespace Engine
{
	class Engine_API World : public Object, public Layer
	{
		template <class T>
		friend class Layerable;

		friend class WorldManager;
		friend class Entity;

	public:
		virtual ~World();

		Event<void, float> OnUpdate = Event<void, float>("World::OnUpdate");
		Event<void, float> OnFixedUpdate = Event<void, float>("World::OnFixedUpdate");
		Event<void, float> OnLateUpdate = Event<void, float>("World::OnLateUpdate");
		Event<void, float> OnDraw = Event<void, float>("World::OnDraw");

		template <class T, class... Args>
		T* Add(Args... Arguments)
		{
			T* Object = new T(*this, Arguments...);
			m_Entities.push_back(Object);

			Object->m_uiID = m_IDManager.Reserve();
			Object->Awake();

			return Object;
		}

		virtual bool Destroy(Entity& Layer);

	protected:
		World(const String& sName = "World");

		virtual void Update(float fDeltaTime);
		virtual void FixedUpdate(float fDeltaTime);

		virtual void Draw(float fDeltaTime);
		virtual void LateUpdate(float fDeltaTime);

		virtual void OnAttach() override;
		virtual void OnDetach() override;

	private:
		virtual bool Remove(Entity& Layer);

		IDManager m_IDManager;

		std::vector<Entity*> m_Entities;
		std::vector<Entity*> m_PendingRemoval;

		uint32_t m_UpdateHandler = UINT_MAX;
		uint32_t m_FixedUpdateHandler = UINT_MAX;
		uint32_t m_DrawHandler = UINT_MAX;
		uint32_t m_LateUpdateHandler = UINT_MAX;
	};
}
