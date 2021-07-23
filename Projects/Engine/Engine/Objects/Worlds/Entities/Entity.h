#pragma once

#include <unordered_map>
#include <cstdint>

#include "Components/Component.h"
#include "Engine/Events/Event.h"

namespace Engine
{
	class World;

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
		
		Component* AddComponent(Component* Comp);
		
		template <class T, class... Args>
		T* AddComponent(Args... Arguments);

		bool RemoveComponent(Component* Comp);
		
		template <class T>
		bool RemoveComponent();

		const Entity* GetParent() const { return Parent; }
		void SetParent(Entity* Object);

		const std::list<Entity*>& GetChildren() const { return Children; }

		Event<void, Component&> OnComponentAdded = Event<void, Component&>("Entity::OnComponentAdded");
		Event<void, Component&> OnComponentRemoved = Event<void, Component&>("Entity::OnComponentRemoved");
		
		Event<void, Entity*, Entity*> OnParentChanged = Event<void, Entity*, Entity*>("Entity::OnParentChanged");
		
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
		Component* GetComponentByID(size_t ID) const
		{
			const auto find = m_Components.find(ID);
			return (find != m_Components.end() ? find->second : nullptr);
		}
		
		std::unordered_map<size_t, Component*> m_Components;
	};
	
	template <class T>
	T* Entity::GetComponent() const
	{
		static_assert(
			std::is_base_of<Component, T>::value,
			"Type is not a Component"
		);
		
		return static_cast<T*>(GetComponentByID(
			Component::GetClassID<T>()
		));
	}
	
	template <class T, class... Args>
	T* Entity::AddComponent(Args... Arguments)
	{
		static_assert(
			std::is_base_of<Component, T>::value,
			"Type is not a Component"
		);
		
		if (T* Comp = GetComponent<T>())
		{
			CB_CORE_WARN("Tried to add component {0} while it already exists!", Comp->GetName());
			return Comp;
		}

		// Call constructor and assign
		T* Comp = new T(*this, Arguments...);
		Comp->m_ID = Component::GetClassID<T>();

		if (!AddComponent(Comp))
			delete Comp;
		
		return Comp;
	}
	
	template <class T>
	bool Entity::RemoveComponent()
	{
		static_assert(
			std::is_base_of<Component, T>::value,
			"Type is not a Component"
		);
		
		auto Find = m_Components.find(Component::GetClassID<T>());
		const bool Found = Find != m_Components.end();

		if (Found)
		{
			if (Find->second)
				OnComponentRemoved(*Find->second);

			m_Components.erase(Find);
		}

		return Found;
	}
}
