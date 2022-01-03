#include "pch.h"
#include "World.h"

#include "Engine/Application.h"
#include "Rendering/Renderers/Renderer2D.h"

namespace Engine
{
	World::World(const String& sName) : Object(sName)
	{
		Application::Get().GetRenderContext().GetAPI().GetRenderer2D()->SetClearColor(SK_ColorTRANSPARENT);
	}

	World::~World()
	{
		// Remove objects
		for (int32_t i = m_PendingRemoval.size() - 1; i >= 0; --i)
		{
			delete m_PendingRemoval[i];
		}

		for (Entity* Entity : m_Entities)
		{
			delete Entity;
		}

		WorldManager::Get().Pop(*this, false);
	}

	void World::Update(float fDeltaTime)
	{
		// Remove objects
		for (uint32_t i = 0; i < m_PendingRemoval.size(); ++i)
		{
			delete m_PendingRemoval[i];
		}

		m_PendingRemoval.clear();

		// Update events
		OnUpdate(fDeltaTime);
	}

	void World::FixedUpdate(float fDeltaTime)
	{
		// Fixed update events
		OnFixedUpdate(fDeltaTime);
	}

	void World::Draw(float fDeltaTime)
	{
		// Fixed update events
		OnDraw(fDeltaTime);
	}

	void World::LateUpdate(float fDeltaTime)
	{
		// Fixed update events
		OnLateUpdate(fDeltaTime);
	}

	void World::OnAttach()
	{
		m_UpdateHandler = Application::Get().OnUpdate.Bind(this, &World::Update);
		m_FixedUpdateHandler = Application::Get().OnFixedUpdate.Bind(this, &World::FixedUpdate);
		m_DrawHandler = Application::Get().OnDraw.Bind(this, &World::Draw);
		m_LateUpdateHandler = Application::Get().OnLateUpdate.Bind(this, &World::LateUpdate);
	}

	void World::OnDetach()
	{
		Application::Get().OnUpdate.Unbind(m_UpdateHandler);
		Application::Get().OnFixedUpdate.Unbind(m_FixedUpdateHandler);
		Application::Get().OnDraw.Unbind(m_DrawHandler);
		Application::Get().OnLateUpdate.Unbind(m_LateUpdateHandler);
	}

	bool World::Remove(Entity& Entity)
	{
		if (&Entity.GetWorld() != this)
			return false;

		if (std::find(m_PendingRemoval.begin(), m_PendingRemoval.end(), &Entity) != m_PendingRemoval.end())
			return false;

		auto it = std::find(m_Entities.begin(), m_Entities.end(), &Entity);
		const bool bResult = it != m_Entities.end();

		if (bResult)
		{
			m_Entities.erase(it);
			m_PendingRemoval.push_back(&Entity);

			m_IDManager.Free(Entity.m_uiID);

			// Entity.OnDestroy();
		}

		return bResult;
	}

	bool World::Destroy(Entity& Entity)
	{
		if (&Entity.GetWorld() != this)
			return false;

		auto it1 = std::find(m_PendingRemoval.begin(), m_PendingRemoval.end(), &Entity);
		if (it1 != m_PendingRemoval.end())
		{
			m_PendingRemoval.erase(it1);
			delete &Entity;

			return true;
		}

		auto it2 = std::find(m_Entities.begin(), m_Entities.end(), &Entity);
		const bool bResult = it2 != m_Entities.end();

		if (bResult)
		{
			m_Entities.erase(it2);
			m_PendingRemoval.push_back(&Entity);

			m_IDManager.Free(Entity.m_uiID);

			// Entity.OnDestroy();
		}

		delete &Entity;
		return bResult;
	}
}
