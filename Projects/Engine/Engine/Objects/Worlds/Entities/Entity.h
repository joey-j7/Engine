#pragma once

#include <unordered_map>
#include <cstdint>

#include "Components/Component.h"
#include "Components/RenderComponent.h"
#include "Engine/Events/Event.h"

#include "Engine/General/Collision.h"

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

		Entity(const Entity&) = delete;
		Entity& operator =(const Entity&) = delete;
		virtual ~Entity();

		virtual const std::string& GetName() const = 0;
		const AABB GetBounds() const;
		
		World* GetWorld() const { return m_pWorld; }
		Type GetType() const { return m_Type; }

		/* Components */
		template <class T>
		T* GetComponent();
		
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

		template <class T>
		T* AddComponent(T* Comp);

		Entity* Parent = nullptr;
		std::list<Entity*> Children;
		
		World* m_pWorld = nullptr;
		uint32_t m_uiID = 0;

		Type m_Type = E_STATIC;

		/* Components */
		Component* GetComponentByID(size_t ID)
		{
			const auto find = m_Components.find(ID);
			return (find != m_Components.end() ? find->second.get() : nullptr);
		}

		std::vector<RenderComponent*> m_RenderComponents = {};
		std::unordered_map<size_t, std::unique_ptr<Component>> m_Components = {};
	};
	
	template <class T>
	T* Entity::GetComponent()
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
		const size_t ID = Component::GetClassID<T>();
		m_Components.insert(std::pair<size_t, std::unique_ptr<Component>>(
			ID, std::move(std::make_unique<T>(*this, Arguments...))
		));
		
		T* Comp = reinterpret_cast<T*>(m_Components.at(ID).get());
		Comp->m_ID = ID;
		
		if (!AddComponent(Comp))
		{
			m_Components.erase(m_Components.find(ID));
			return nullptr;
		}

		if constexpr (std::is_base_of<RenderComponent, T>::value)
		{
			m_RenderComponents.push_back(Comp);
		}
		
		return Comp;
	}

	template <class T>
	T* Entity::AddComponent(T* Comp)
	{
		bool Prohibited = false;

		// Check for prohibited components for dependency components
		for (auto& Pair : Comp->m_PendingComponentsToAdd)
		{
			if (!Pair.second || GetComponentByID(Pair.second->GetID()))
				continue;
		
			if (!Pair.second->IsCompatible(Comp->GetID()) || !Comp->IsCompatible(Pair.second->GetID()))
			{
				CB_CORE_ERROR(
					"Cannot add component {0} to entity {1} because of conflicting component {2}!",
					Comp->GetName(), GetName(), Pair.second->GetName()
				);
		
				Prohibited = true;
			}
		
			for (auto& CompPair : m_Components)
			{
				if (!Pair.second->IsCompatible(CompPair.second->GetID()) ||
					!CompPair.second.get()->IsCompatible(Pair.second->GetID())
				) {
					CB_CORE_ERROR(
						"Cannot add component {0} to entity {1} because of conflicting component {2}!",
						Pair.second->GetName(), GetName(), CompPair.second.get()->GetName()
					);
			
					Prohibited = true;
				}
			}
		}

		// Check for prohibited components for main component
		for (auto& Pair : m_Components)
		{
			if (!Comp->IsCompatible(Pair.second->GetID()) || !Pair.second.get()->IsCompatible(Comp->GetID()))
			{
				CB_CORE_ERROR(
					"Cannot add component {0} to entity {1} because of conflicting component {2}!",
					Comp->GetName(), GetName(), Pair.second.get()->GetName()
				);
		
				Prohibited = true;
			}
		}

		if (Prohibited)
		{
			Comp->m_PendingComponentsToAdd.clear();
			return nullptr;
		}
		
		for (auto& Pair : Comp->m_PendingComponentsToAdd)
		{
			if (!Pair.second)
				continue;
		
			if (!GetComponentByID(Pair.second->GetID()))
			{
				CB_CORE_INFO(
					"Added component {0} to entity {1} as a dependency for component {2}",
					Pair.second->GetName(), GetName(), Comp->GetName()
				);
		
				Pair.second->m_ID = Pair.first;
				m_Components.insert(std::pair<size_t, std::unique_ptr<Component>>(
					Pair.first, std::move(Pair.second)
				));
			}
		}
		
		Comp->m_PendingComponentsToAdd.clear();
		OnComponentAdded(*Comp);

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
			
			if constexpr (std::is_base_of<RenderComponent, T>::value)
				m_RenderComponents.erase(&Find->second);
			
			m_Components.erase(Find);
		}
		
		return Found;
	}
}
