#pragma once

#include <atomic>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include <typeinfo>

#include "Engine/Objects/Object.h"
#include "Engine/Events/Event.h"

namespace Engine
{
	class World;
	class Component;

	extern std::atomic_uint32_t TypeIdCounter;

	class Engine_API Entity : public Object
	{
	public:
		friend class World;
		
		enum Type
		{
			E_STATIC,
			E_DYNAMIC
		};

		Entity(const std::string& sName = "Unnamed Entity") : Object(sName) {}
		virtual ~Entity();
		
		World* GetWorld() const { return m_pWorld; }
		Type GetType() const { return m_Type; }

		/* Components */
		template <class T>
		T* GetComponent();

		template <class T>
		T& AddComponent();

		template <class T>
		bool RemoveComponent();

		const Entity* GetParent() const { return Parent; }
		void SetParent(Entity* Object);

		const std::list<Entity*>& GetChildren() const { return Children; }

		Event<void, Entity*, Entity*> OnParentChange;
		
	protected:
		virtual void Awake() = 0;
		virtual bool OnDestroy() { return true; }

		Entity* Parent = nullptr;
		std::list<Entity*> Children;
		
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
			if (!obj->HasForcedUniqueness())
				return *obj;
		}

		std::unique_ptr<T> Component = std::make_unique<T>();

		// Check for prohibited components first
		bool HasIllegalComponents = false;

		if (Component->HasForcedUniqueness())
		{
			if (GetComponent<T>())
			{
				CB_CORE_ERROR("Tried to add unique component {0} while it already exists!", typeid(T).name());
				HasIllegalComponents = true;
			}
		}

		if (HasIllegalComponents)
		{
			Component.reset();
			return nullptr;
		}

		for (std::type_info TypeInfo : Component->GetProhibitedComponents())
		{
			if (GetComponent<decltype(TypeInfo)>())
			{
				CB_CORE_ERROR("Tried to add component {0} while the entity contains incompatible component {1}!", typeid(T).name(), TypeInfo.name());
				HasIllegalComponents = true;
			}
		}

		if (HasIllegalComponents)
		{
			Component.reset();
			return nullptr;
		}

		// Add dependency components second
		for (std::type_info TypeInfo : Component->GetDependencyComponents())
		{
			if (!GetComponent<decltype(TypeInfo)>())
			{
				CB_CORE_INFO("Added dependency component {0} for component {1}", TypeInfo.name(), typeid(T).name());
				AddComponent<decltype(TypeInfo)>();
			}
		}

		// Now add component
		m_Components.insert(GetComponentID<T>(), Component);

		return *Component.get();
	}

	template <class T>
	bool Entity::RemoveComponent()
	{
		static_assert(std::is_base_of<Component, T>::value);
		
		auto find = m_Components.find(GetComponentID<T>());
		return find != m_Components.end() ? m_Components.erase(find) : false;
	}
}
