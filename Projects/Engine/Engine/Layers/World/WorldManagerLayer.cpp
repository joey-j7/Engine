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
			m_Worlds.push_back(pWorld);
		}
	}

	void WorldManagerLayer::Remove(World* pWorld)
	{
		auto it = std::find(m_Worlds.begin(), m_Worlds.end(), pWorld);

		if (it != m_Worlds.end())
		{
			m_Worlds.erase(it);
			delete pWorld;
		}
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
