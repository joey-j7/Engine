#pragma once

#include "Engine/Layers/Layer.h"
#include "Engine/Objects/Worlds/World.h"

namespace Engine
{
	class Engine_API WorldManagerLayer : public Layer
	{
		friend class World;

	public:
		WorldManagerLayer();
		~WorldManagerLayer();

		// Removes all worlds and replaces it with new specified world
		template <class T, class... Args>
		void Clear(Args... Arguments)
		{
			static_assert(
				std::is_base_of<World, T>::value,
				"Type is not a World"
			);

			for (
				auto i = m_Worlds.rbegin();
				i != m_Worlds.rend(); ++i
			) {
				delete *i;
			}

			m_Worlds.clear();

			T* World = new T(Arguments...);
		}

		// Removes current world and switches to the new specified world
		template <class T, class... Args>
		void Switch(Args... Arguments)
		{
			static_assert(
				std::is_base_of<World, T>::value,
				"Type is not a World"
			);

			if (GetActive())
				delete GetActive();

			T* World = new T(Arguments...);
		}

		const std::vector<World*>& GetWorlds() const { return m_Worlds; };
		World* GetActive() const { return m_Worlds.empty() ? nullptr : m_Worlds.back(); };

	private:
		void Add(World* pWorld);
		void Remove(World* pWorld);

		virtual void OnAttach(const LayerStack& stack) override;
		virtual bool OnDetach(const LayerStack& stack) override;

		virtual void Update(float fDeltaTime) override;
		virtual void FixedUpdate(float fDeltaTime) override;

		virtual void Draw(float fDeltaTime) override;
		virtual void LateUpdate(float fDeltaTime) override;

		std::vector<World*> m_Worlds;
	};
}
