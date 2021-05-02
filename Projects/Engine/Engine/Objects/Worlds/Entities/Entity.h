#pragma once

#include <atomic>
#include <unordered_map>
#include <memory>
#include <cstdint>

namespace Engine
{
	class World;
	class Component;

	extern std::atomic_uint32_t TypeIdCounter;

	class Engine_API Entity
	{
	public:
		friend class World;
		
		enum Type
		{
			E_STATIC,
			E_DYNAMIC
		};

		Entity() {}
		virtual ~Entity() {}
		
		World* GetWorld() const { return m_pWorld; }
		Type GetType() const { return m_Type; }

		/* Components */
		template <class T>
		T* GetComponent();

		template <class T>
		T& AddComponent();

		template <class T>
		bool RemoveComponent();

	protected:
		virtual void Awake() = 0;
		virtual bool OnDestroy() { return true; }

		World* m_pWorld = nullptr;
		uint32_t m_uiID = 0;

		Type m_Type = E_STATIC;

		/* Components */
		template <class T>
		static uint32_t GetComponentID();

		std::unordered_map<uint32_t, Component*> m_Components;
	};

	template <class T>
	uint32_t Entity::GetComponentID()
	{
		static_assert(std::is_base_of<Component, T>::value);
		
		static uint32_t id = ++TypeIdCounter;
		return id;
	}

	template <class T>
	T* Entity::GetComponent()
	{
		static_assert(std::is_base_of<Component, T>::value);

		auto find = m_Components.find(GetComponentID<T>());
		return find != m_Components.end() ? find.second : nullptr;
	}

	template <class T>
	T& Entity::AddComponent()
	{
		static_assert(std::is_base_of<Component, T>::value);
		
		if (T* obj = GetComponent<T>())
		{
			return *obj;
		}

		std::unique_ptr<T> component = std::make_unique<T>();
		m_Components.insert(GetComponentID<T>(), component);

		return *component.get();
	}

	template <class T>
	bool Entity::RemoveComponent()
	{
		static_assert(std::is_base_of<Component, T>::value);
		
		auto find = m_Components.find(GetComponentID<T>());
		return find != m_Components.end() ? m_Components.erase(find) : false;
	}
}