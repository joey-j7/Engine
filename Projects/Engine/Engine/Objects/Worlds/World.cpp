#include "pch.h"
#include "World.h"

#include "Engine/Application.h"
#include "Entities/DynamicEntity.h"

namespace Engine
{
	World::World(const String& sName) : LayeredObject(sName)
	{
		Application::Get().GetWorldManager().Add(this);
	}

	World::~World()
	{
		while (!m_pObjectsToRemove.empty())
		{
			delete m_pObjectsToRemove.back();
			m_pObjectsToRemove.pop_back();
		}

		while (!m_pWorldObjects.empty())
		{
			delete m_pWorldObjects.front();
		}

		Application::Get().GetWorldManager().Remove(this);
	}

	void World::Update(float fDeltaTime)
	{
		// Remove objects
		for (uint32_t i = 0; i < m_pObjectsToRemove.size(); ++i)
		{
			delete m_pObjectsToRemove[i];
		}

		m_pObjectsToRemove.clear();

		// Add objects
		for (DynamicEntity* pEntity : m_pObjectsToActivate)
		{
			pEntity->Play();
		}

		m_pObjectsToActivate.clear();

		// Update all objects
		for (DynamicEntity* pObject : m_pDynamicWorldObjects)
		{
			if (!pObject->IsActive())
				continue;

			pObject->Update(fDeltaTime);
		}

		// Update layers
		LayeredObject::Update(fDeltaTime);
	}

	void World::FixedUpdate(float fDeltaTime)
	{
		for (DynamicEntity* pObject : m_pDynamicWorldObjects)
		{
			if (!pObject->IsActive())
				continue;

			pObject->FixedUpdate(fDeltaTime);
		}

		// Update layers
		LayeredObject::FixedUpdate(fDeltaTime);
	}

	void World::Draw(float fDeltaTime)
	{
		for (DynamicEntity* pObject : m_pDynamicWorldObjects)
		{
			if (pObject->IsStopped())
				continue;

			pObject->Draw(fDeltaTime);
		}

		// Update layers
		LayeredObject::Draw(fDeltaTime);
	}

	void World::LateUpdate(float fDeltaTime)
	{
		for (DynamicEntity* pObject : m_pDynamicWorldObjects)
		{
			if (!pObject->IsActive())
				continue;

			pObject->LateUpdate(fDeltaTime);
		}

		// Update layers
		LayeredObject::LateUpdate(fDeltaTime);
	}

	bool World::Pause()
	{
		if (!LayeredObject::Pause())
			return false;

		for (DynamicEntity* pObject : m_pDynamicWorldObjects)
		{
			pObject->Pause();
		}

		return true;
	}

	bool World::Resume()
	{
		if (!LayeredObject::Resume())
			return false;

		for (DynamicEntity* pObject : m_pDynamicWorldObjects)
		{
			pObject->Resume();
		}

		return true;
	}

	bool World::Stop()
	{
		if (!LayeredObject::Stop())
			return false;

		for (DynamicEntity* pObject : m_pDynamicWorldObjects)
		{
			pObject->Stop();
		}

		return true;
	}

	void World::Add(Entity* pObject)
	{
		if (pObject->GetWorld())
			return;

		if (std::find(m_pObjectsToActivate.begin(), m_pObjectsToActivate.end(), pObject) != m_pObjectsToActivate.end())
			return;

		if (std::find(m_pWorldObjects.begin(), m_pWorldObjects.end(), pObject) != m_pWorldObjects.end())
			return;

		m_pWorldObjects.push_back(pObject);

		pObject->m_uiID = m_IDManager.Reserve();
		pObject->m_pWorld = this;

		if (pObject->GetType() == Entity::E_DYNAMIC)
		{
			DynamicEntity* pDynamicObject = static_cast<DynamicEntity*>(pObject);
			
			m_pDynamicWorldObjects.push_back(pDynamicObject);
			m_pObjectsToActivate.push_back(pDynamicObject);

			pDynamicObject->Stop();
		}
		
		pObject->Awake();
	}

	bool World::Remove(Entity* pObject)
	{
		if (pObject->m_pWorld != this)
			return false;

		if (std::find(m_pObjectsToRemove.begin(), m_pObjectsToRemove.end(), pObject) != m_pObjectsToRemove.end())
			return false;

		auto it = std::find(m_pWorldObjects.begin(), m_pWorldObjects.end(), pObject);
		const bool bResult = it != m_pWorldObjects.end();

		if (bResult)
		{
			m_pWorldObjects.erase(it);
			m_pObjectsToRemove.push_back(pObject);

			// Remove from activation list
			if (pObject->GetType() == Entity::E_DYNAMIC)
			{
				const DynamicEntity* pDynamicObject = static_cast<DynamicEntity*>(pObject);
				const auto it2 = std::find(m_pObjectsToActivate.begin(), m_pObjectsToActivate.end(), pDynamicObject);

				if (it2 != m_pObjectsToActivate.end())
				{
					m_pObjectsToActivate.erase(it2);
					m_pObjectsToRemove.push_back(pObject);
				}
			}

			pObject->OnDestroy();
		}

		return bResult;
	}
}
