#pragma once

#include "Engine/Layers/Layer.h"
#include "Engine/Objects/Worlds/World.h"

namespace Engine
{
	class Engine_API WorldManagerLayer : public Layer
	{
	public:
		WorldManagerLayer();
		~WorldManagerLayer();

		void Add(World* pWorld);
		void Remove(World* pWorld);

		World* GetActive() const { return m_Worlds.empty() ? nullptr : m_Worlds.back(); };

	private:
		virtual void OnAttach(const LayerStack& stack) override;
		virtual bool OnDetach(const LayerStack& stack) override;

		virtual void Update(float fDeltaTime) override;
		virtual void FixedUpdate(float fDeltaTime) override;

		virtual void Draw(float fDeltaTime) override;
		virtual void LateUpdate(float fDeltaTime) override;

		std::vector<World*> m_Worlds;
	};
}
