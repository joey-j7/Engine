#pragma once

#include <atomic>
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

		Entity() {}
		virtual ~Entity();

		virtual const std::string& GetName() const = 0;
		
		World* GetWorld() const { return m_pWorld; }
		Type GetType() const { return m_Type; }

		/* Components */
		template <class T>
		T* GetComponent() const;
		
		const std::unordered_map<uint32_t, Component*>& GetComponents() const
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

		Component* GetComponentByID(uint32_t ID) const
		{
			const auto find = m_Components.find(ID);
			return (find != m_Components.end() ? find->second : nullptr);
		}
		
		std::unordered_map<uint32_t, Component*> m_Components;

	private:
		std::unordered_map<uint32_t, Component*> m_PendingComponentsToAdd;
		static std::atomic_uint32_t TypeIdCounter;
	};
	
	template <class T>
	uint32_t Entity::GetComponentID()
	{
		static uint32_t id = ++TypeIdCounter;
		return id;
	}
	
	template <class T>
	T* Entity::GetComponent() const
	{
		return reinterpret_cast<T*>(GetComponentByID(GetComponentID<T>()));
	}
	
	template <class T>
	T* Entity::AddComponent()
	{
		if (T* obj = GetComponent<T>())
		{
			CB_CORE_WARN("Tried to add component {0} while it already exists!", obj->GetName());
			return obj;
		}

		// Call constructor and assign
		T* Obj = new T(*this);
		Obj->m_ID = GetComponentID<T>();

		// Now add component
		m_Components.insert(std::pair<uint32_t, Component*>(GetComponentID<T>(), Obj));

		// Check for prohibited components
		bool Prohibited = Obj->m_Prohibited;

		if (!Prohibited)
		{
			for (auto& Pair : m_PendingComponentsToAdd)
			{
				if (!Pair.second)
					continue;

				if (Pair.second->m_Prohibited)
				{
					Prohibited = true;
					break;
				}
			}
		}

		if (Prohibited)
		{
			m_PendingComponentsToAdd.clear();
			m_Components.erase(m_Components.find(Obj->m_ID));
			
			delete Obj;
			return nullptr;
		}

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

		/*for (auto& Pair : GetComponents())
		{
			if (!Pair.second->IsCompatible(Obj))
			{
				CB_CORE_ERROR(
					"Cannot add component {0} to entity {1} because of conflicting component {2}!",
					Obj->GetName(), GetName(), Pair.second->GetName()
				);

				m_Components.erase(m_Components.find(Obj));
				delete Obj;
				return nullptr;
			}
		}*/

		return Obj;
	}
	
	template <class T>
	bool Entity::RemoveComponent()
	{
		auto find = m_Components.find(GetComponentID<T>());
		return find != m_Components.end() ? m_Components.erase(find) : false;
	}
}
