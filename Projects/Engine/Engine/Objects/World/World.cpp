#include "pch.h"
#include "World.h"

namespace Engine
{
	World::World(const std::string& sName) : LayeredObject(sName)
	{

	}

	World::~World()
	{
		for (Entity* pObject : m_pObjectsToAdd)
		{
			delete pObject;
		}

		for (Entity* pObject : m_pObjectsToRemove)
		{
			delete pObject;
		}

		for (Entity* pObject : m_pWorldObjects)
		{
			delete pObject;
		}
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
		for (Entity* pEntity : m_pObjectsToAdd)
		{
			pEntity->Play();
		}

		m_pObjectsToAdd.clear();

		// Update all objects
		for (Entity* pObject : m_pWorldObjects)
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
		for (Entity* pObject : m_pWorldObjects)
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
		for (Entity* pObject : m_pWorldObjects)
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
		for (Entity* pObject : m_pWorldObjects)
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

		for (Entity* pObject : m_pWorldObjects)
		{
			pObject->Pause();
		}

		return true;
	}

	bool World::Resume()
	{
		if (!LayeredObject::Resume())
			return false;

		for (Entity* pObject : m_pWorldObjects)
		{
			pObject->Resume();
		}

		return true;
	}

	bool World::Stop()
	{
		if (!LayeredObject::Stop())
			return false;

		for (Entity* pObject : m_pWorldObjects)
		{
			pObject->Stop();
		}

		return true;
	}

	void World::Add(Entity* pObject)
	{
		if (pObject->GetWorld())
			return;

		if (std::find(m_pObjectsToAdd.begin(), m_pObjectsToAdd.end(), pObject) != m_pObjectsToAdd.end())
			return;

		if (std::find(m_pWorldObjects.begin(), m_pWorldObjects.end(), pObject) != m_pWorldObjects.end())
			return;

		m_pObjectsToAdd.push_back(pObject);
		m_pWorldObjects.push_back(pObject);

		pObject->m_uiID = m_IDManager.Reserve();
		pObject->m_pWorld = this;

		pObject->Stop();
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

			it = std::find(m_pObjectsToAdd.begin(), m_pObjectsToAdd.end(), pObject);

			if (it != m_pObjectsToAdd.end())
			{
				m_pObjectsToAdd.erase(it);
				m_pObjectsToRemove.push_back(pObject);
			}

			pObject->Destroy();
		}

		return bResult;
	}
}