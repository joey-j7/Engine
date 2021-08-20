#pragma once

#include <unordered_map>
#include <cstdint>
#include <vector>

#include "Engine/Objects/Object.h"
#include "Engine/Objects/Worlds/Entities/Components/RegisterComponent.h"

#include "Engine/Events/Event.h"
#include "Engine/General/Collision.h"

#include "Engine/Logger/Log.h"

namespace Engine
{
	class World;
	
	class Transform2DComponent;
	class Transform3DComponent;

	class Component;
	
	class Engine_API Entity
	{
	public:
		friend class World;
		friend class Entity;
		friend class Component;
		
		enum Type
		{
			E_STATIC,
			E_DYNAMIC
		};

		Entity(const Entity&) = delete;
		Entity& operator =(const Entity&) = delete;
		virtual ~Entity();

		virtual const String& GetName() const = 0;
		const AABB GetBounds() const;
		
		World* GetWorld() const { return m_pWorld; }
		Type GetType() const { return m_Type; }

		/* Components */
		template <class T>
		T* GetComponent();
		
		template <class T>
		std::vector<T*> GetComponentsOfType() const;

		template <class T, class... Args>
		T* AddComponent(Args... Arguments);

		bool RemoveComponent(Component* Comp);
		
		template <class T>
		bool RemoveComponent();

		Entity* GetParent() const { return m_Parent; }
		void SetParent(Entity* Object);

		const std::vector<Entity*>& GetChildren() const { return m_Children; }

		Event<void, Entity&, Component&> OnComponentAdded = Event<void, Entity&, Component&>("Entity::OnComponentAdded");
		Event<void, Entity&, Component&> OnComponentRemoved = Event<void, Entity&, Component&>("Entity::OnComponentRemoved");

		Event<void, Entity&, Entity&> OnChildAdded = Event<void, Entity&, Entity&>("Entity::OnChildAdded");
		Event<void, Entity&, Entity&> OnChildRemoved = Event<void, Entity&, Entity&>("Entity::OnChildRemoved");
		
		Event<void, Entity&, Entity*, Entity*> OnParentChanged = Event<void, Entity&, Entity*, Entity*>("Entity::OnParentChanged");

	protected:
		Entity() {}
		
		virtual void Awake() = 0;
		virtual bool OnDestroy() { return true; }

		template <class T>
		T* AddComponent(T* Comp);

		void NotifyChildrenOnParentChanged(Entity& Origin, Entity* Old, Entity* New);

		Entity* m_Parent = nullptr;
		std::vector<Entity*> m_Children;
		
		World* m_pWorld = nullptr;
		uint32_t m_uiID = 0;

		Type m_Type = E_STATIC;

		/* Components */
		Component* GetComponentByID(size_t ID)
		{
			const auto find = m_Components.find(ID);
			return (find != m_Components.end() ? find->second.get() : nullptr);
		}
		
		std::unordered_map<size_t, std::unique_ptr<Component>> m_Components = {};
	};

	class Engine_API Component : public Object
	{
		friend class Entity;

	public:
		Component(Entity& Entity, const String& sName = "Component") : Object(sName), m_Entity(Entity) {}
		Component(const Component&) = delete;
		Component& operator =(const Component&) = delete;

		virtual ~Component() = default;

		Entity& GetEntity() const { return m_Entity; }
		bool HasForcedUniqueness() const { return m_ForceUniqueness; }

		bool IsCompatible(size_t ComponentID) const
		{
			return std::find(
				m_ProhibitedTypes.begin(),
				m_ProhibitedTypes.end(),
				ComponentID
			) == m_ProhibitedTypes.end();
		}

		bool HasDependency(size_t ComponentID) const
		{
			return std::find(
				m_DependencyTypes.begin(),
				m_DependencyTypes.end(),
				ComponentID
			) != m_DependencyTypes.end();
		}

		template <typename D>
		D* GetDependency() const
		{
			D* Comp = GetEntity().template GetComponent<D>();

			// Not added yet
			if (!Comp)
				Comp = static_cast<D*>(
					m_PendingComponentsToAdd.at(
						Component::GetClassID<D>()
					).get()
					);

			return Comp;
		}

		template <class T>
		static size_t GetClassID();

		size_t GetID() const { return m_ID; }

	protected:
		Entity& m_Entity;

		bool m_ForceUniqueness = false;

		virtual void OnInitialized() {}

		template <typename... ProhibitedTypes>
		constexpr void AddProhibitedTypes();

		template <typename... DependencyTypes>
		constexpr void AddDependencyTypes();

	private:
		std::vector<size_t> m_DependencyTypes;
		std::vector<size_t> m_ProhibitedTypes;

		std::unordered_map<size_t, std::unique_ptr<Component>> m_PendingComponentsToAdd;

		size_t m_ID = 0;
	};

	template <typename ... ProhibitedTypes>
	constexpr void Component::AddProhibitedTypes()
	{
		m_ProhibitedTypes.reserve(m_ProhibitedTypes.size() + sizeof...(ProhibitedTypes));
		const void* Add[sizeof...(ProhibitedTypes)] = {
			[&]()->void*
			{
				static_assert(
					std::is_base_of<Component, ProhibitedTypes>::value,
					"Type is not a Component"
				);

				const size_t ID = GetClassID<ProhibitedTypes>();

				if (std::find(m_ProhibitedTypes.begin(), m_ProhibitedTypes.end(), ID) != m_ProhibitedTypes.end())
					return nullptr;

				m_ProhibitedTypes.push_back(ID);
				return nullptr;
			}() ...
		};
	}

	template <typename ... DependencyTypes>
	constexpr void Component::AddDependencyTypes()
	{
		m_DependencyTypes.reserve(m_DependencyTypes.size() + sizeof...(DependencyTypes));
		const void* Add[sizeof...(DependencyTypes)] = {
			[&]()->void*
			{
				static_assert(
					std::is_base_of<Component, DependencyTypes>::value,
					"Type is not a Component"
				);

				size_t ID = GetClassID<DependencyTypes>();

				if (m_PendingComponentsToAdd.find(ID) != m_PendingComponentsToAdd.end())
					return nullptr;

				m_DependencyTypes.push_back(ID);

				if (GetEntity().template GetComponent<DependencyTypes>())
					return nullptr;

				m_PendingComponentsToAdd.emplace(
					std::pair<size_t, std::unique_ptr<Component>>(
						ID,
						nullptr
					)
				);

				std::unique_ptr<DependencyTypes> Comp = std::make_unique<DependencyTypes>(m_Entity);
				Comp->m_ID = ID;
				m_PendingComponentsToAdd[ID] = std::move(Comp);

				return nullptr;
			}() ...
		};
	}

	template <class T>
	size_t Component::GetClassID()
	{
		static_assert(
			std::is_base_of<Component, T>::value,
			"Type is not a Component"
			);

		return typeid(T).hash_code();
	}
	
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

	template <class T>
	std::vector<T*> Entity::GetComponentsOfType() const
	{
		std::vector<T*> Components;

		for (auto& Pair : m_Components)
		{
			if (!dynamic_cast<T*>(Pair.second.get()))
				continue;

			Components.push_back(reinterpret_cast<T*>(Pair.second.get()));
		}

		return Components;
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

				Pair.second.release();
			}
		}
		
		Comp->m_PendingComponentsToAdd.clear();
		OnComponentAdded(*this, *Comp);

		Comp->OnInitialized();

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
			{
				OnComponentRemoved(*this, *Find->second);
			}

			CB_CORE_TRACE("Removed component {0} from {1}", Find->second ? Find->second->GetName() : "nullptr", GetName());
			
			m_Components.erase(Find);

			// Remove all components that depend on this
			for (auto& Comp : m_Components)
			{
				if (!Comp.second->HasDependency(Component::GetClassID<T>()))
					continue;

				RemoveComponent(Comp.second.get());
			}
		}
		
		return Found;
	}
}
