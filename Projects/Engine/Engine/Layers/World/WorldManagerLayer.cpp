#include "pch.h"
#include "WorldManagerLayer.h"

#include "Engine/Application.h"

namespace Engine
{
	WorldManagerLayer::WorldManagerLayer()
		: Layer("WorldManagerLayer")
	{
	}

	WorldManagerLayer::~WorldManagerLayer()
	{
		for (World* pWorld : m_Worlds)
		{
			delete pWorld;
		}
	}

	void WorldManagerLayer::Add(World* pWorld)
	{
		auto it = std::find(m_Worlds.begin(), m_Worlds.end(), pWorld);

		if (it == m_Worlds.end())
		{
			if (GetActive())
				GetActive()->Pause();

			m_Worlds.push_back(pWorld);
			CB_CORE_TRACE("Added world {0}, total world count: {1}", pWorld->GetName(), m_Worlds.size());
		}
	}

	void WorldManagerLayer::Remove(World* pWorld)
	{
		auto it = std::find(m_Worlds.begin(), m_Worlds.end(), pWorld);

		if (it != m_Worlds.end())
		{
			CB_CORE_TRACE("Removing world {0}, total world count: {1}", pWorld->GetName(), m_Worlds.size() - 1);
			m_Worlds.erase(it);
		}

		if (GetActive())
			GetActive()->Play();
	}

	void WorldManagerLayer::OnAttach(const LayerStack& stack)
	{
		Layer::OnAttach(stack);

		for (World* pWorld : m_Worlds)
		{
			pWorld->Resume();
		}
	}

	bool WorldManagerLayer::OnDetach(const LayerStack& stack)
	{
		if (!Layer::OnDetach(stack))
			return false;

		for (World* pWorld : m_Worlds)
		{
			pWorld->Pause();
		}

		return true;
	}

	void WorldManagerLayer::Update(float fDeltaTime)
	{
		for (World* pWorld : m_Worlds)
		{
			if (!pWorld->IsActive())
				continue;

			pWorld->Update(fDeltaTime);
		}
	}

	void WorldManagerLayer::FixedUpdate(float fDeltaTime)
	{
		for (World* pWorld : m_Worlds)
		{
			if (!pWorld->IsActive())
				continue;

			pWorld->FixedUpdate(fDeltaTime);
		}
	}

	void WorldManagerLayer::Draw(float fDeltaTime)
	{
		for (World* pWorld : m_Worlds)
		{
			if (!pWorld->IsActive())
				continue;

			pWorld->Draw(fDeltaTime);
		}
	}

	void WorldManagerLayer::LateUpdate(float fDeltaTime)
	{
		for (World* pWorld : m_Worlds)
		{
			if (!pWorld->IsActive())
				continue;

			pWorld->LateUpdate(fDeltaTime);
		}
	}
}
