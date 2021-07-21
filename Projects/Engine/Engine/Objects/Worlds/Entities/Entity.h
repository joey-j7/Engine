#pragma once

#include <unordered_map>
#include <cstdint>

#include "Engine/Events/Event.h"

namespace Engine
{
	class World;
	class Component;
	
	class Engine_API Entity
	{
	public:
		friend class World;
		friend class Component;
		
		enum Type
		{
			E_STATIC,
			E_DYNAMIC
		};

		virtual ~Entity();

		virtual const std::string& GetName() const = 0;
		
		World* GetWorld() const { return m_pWorld; }
		Type GetType() const { return m_Type; }

		/* Components */
		template <class T>
		T* GetComponent() const;
		
		const std::unordered_map<size_t, Component*>& GetComponents() const
		{
			return m_Components;
		}
		
		template <class T>
		T* AddComponent();

		template <class T>
		bool RemoveComponent();

		const Entity* GetParent() const { return Parent; }
		void SetParent(Entity* Object);

		const std::list<Entity*>& GetChildren() const { return Children; }

		Event<void, Entity*, Entity*> OnParentChange;
		
	protected:
		Entity() {}
		
		virtual void Awake() = 0;
		virtual bool OnDestroy() { return true; }

		Entity* Parent = nullptr;
		std::list<Entity*> Children;
		
		World* m_pWorld = nullptr;
		uint32_t m_uiID = 0;

		Type m_Type = E_STATIC;

		/* Components */
		template <class T>
		static size_t GetComponentID();

		Component* GetComponentByID(size_t ID) const
		{
			const auto find = m_Components.find(ID);
			return (find != m_Components.end() ? find->second : nullptr);
		}
		
		std::unordered_map<size_t, Component*> m_Components;

	private:
		std::unordered_map<size_t, Component*> m_PendingComponentsToAdd;
	};
	
	template <class T>
	size_t Entity::GetComponentID()
	{
		static_assert(
			std::is_base_of<Component, T>::value,
			"Type is not a Component"
		);
		
		return typeid(T).hash_code();
	}
	
	template <class T>
	T* Entity::GetComponent() const
	{
		static_assert(
			std::is_base_of<Component, T>::value,
			"Type is not a Component"
		);
		
		return static_cast<T*>(GetComponentByID(GetComponentID<T>()));
	}
	
	template <class T>
	T* Entity::AddComponent()
	{
		static_assert(
			std::is_base_of<Component, T>::value,
			"Type is not a Component"
		);
		
		if (T* obj = GetComponent<T>())
		{
			CB_CORE_WARN("Tried to add component {0} while it already exists!", obj->GetName());
			return obj;
		}

		// Call constructor and assign
		T* Obj = new T(*this);
		Obj->m_ID = GetComponentID<T>();

		bool Prohibited = false;
		
		// Check for prohibited components for dependency components
		for (auto& Pair : m_PendingComponentsToAdd)
		{
			if (!Pair.second)
				continue;

			if (!Pair.second->IsCompatible(Obj) || !Obj->IsCompatible(Pair.second))
			{
				CB_CORE_ERROR(
					"Cannot add component {0} to entity {1} because of conflicting component {2}!",
					Obj->GetName(), GetName(), Pair.second->GetName()
				);

				Prohibited = true;
			}

			for (auto& Comp : m_Components)
			{
				if (!Pair.second->IsCompatible(Comp.second) || !Comp.second->IsCompatible(Pair.second))
				{
					CB_CORE_ERROR(
						"Cannot add component {0} to entity {1} because of conflicting component {2}!",
						Pair.second->GetName(), GetName(), Comp.second->GetName()
					);

					Prohibited = true;
				}
			}
		}

		// Check for prohibited components for main component
		for (auto& Comp : m_Components)
		{
			if (!Obj->IsCompatible(Comp.second) || !Comp.second->IsCompatible(Obj))
			{
				CB_CORE_ERROR(
					"Cannot add component {0} to entity {1} because of conflicting component {2}!",
					Obj->GetName(), GetName(), Comp.second->GetName()
				);

				Prohibited = true;
			}
		}

		if (Prohibited)
		{
			for (auto Pair : m_PendingComponentsToAdd)
			{
				if (Pair.second)
					delete Pair.second;
			}
			
			m_PendingComponentsToAdd.clear();
			
			delete Obj;
			return nullptr;
		}

		// Now add component
		m_Components.insert(std::pair<size_t, Component*>(Obj->m_ID, Obj));

		for (auto& Pair : m_PendingComponentsToAdd)
		{
			if (!Pair.second)
				continue;
			
			CB_CORE_INFO(
				"Added component {0} to entity {1} as a dependency for component {2}",
				Pair.second->GetName(), GetName(), Obj->GetName()
			);

			Pair.second->m_ID = Pair.first;
			m_Components.insert(Pair);
		}

		m_PendingComponentsToAdd.clear();
		return Obj;
	}
	
	template <class T>
	bool Entity::RemoveComponent()
	{
		static_assert(
			std::is_base_of<Component, T>::value,
			"Type is not a Component"
		);
		
		auto find = m_Components.find(GetComponentID<T>());
		return find != m_Components.end() ? m_Components.erase(find) : false;
	}
}
