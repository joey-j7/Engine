#pragma once

#include <unordered_map>
#include <cstdint>
#include <vector>

#include "Engine/Objects/Object.h"

#include "Engine/Events/Event.h"
#include "Engine/General/Collision.h"

#include "Engine/Objects/Layers/Layer.h"

#include "Engine/Logger/Log.h"

#include <include/core/SkMatrix.h>

namespace Engine
{
	class World;
	
	class Transform2DComponent;
	class Transform3DComponent;

	class Component;
	
	class Engine_API Entity : public Object
	{
	public:
		friend class World;
		friend class Entity;
		friend class Component;

		template <class T>
		friend class Layerable;

		enum Visibility
		{
			E_VISIBLE = 0,
			E_HIDDEN = 1,
			E_COLLAPSED = 2
		};

		Entity(World& World, const String& sName = "Entity");
		
		Entity(const Entity&) = delete;
		Entity& operator =(const Entity&) = delete;
		virtual ~Entity();

		const AABB GetBounds() const;

		// TODO: Change to default Matrix class
		const SkMatrix GetMatrix() const;
		World& GetWorld() const { return *m_World; }

		Visibility GetVisibility() const;
		void SetVisibility(Visibility Visibility);

		/* Components */
		template <class T>
		T* GetComponent(bool SearchPending = true);
		
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

		Event<void, Visibility> OnVisibilityChanged = Event<void, Visibility>("Entity::OnVisibilityChanged");

	protected:
		virtual void Awake() {};

		template <class T>
		T* AddComponent(T* Comp);

		void NotifyChildrenOnParentChanged(Entity& Origin, Entity* Old, Entity* New);
		void NotifyChildrenOnVisibilityChanged(Visibility Vis);

		Entity* m_Parent = nullptr;
		std::vector<Entity*> m_Children;
		
		World* m_World = nullptr;
		uint32_t m_uiID = 0;

		Visibility m_Visibility = E_VISIBLE;

		/* Components */
		Component* GetComponentByID(size_t ID, bool SearchPending = true)
		{
			auto find = m_Components.find(ID);

			if (find != m_Components.end())
				return find->second.get();

			if (SearchPending)
			{
				find = m_PendingComponentsToAdd.find(ID);

				if (find != m_PendingComponentsToAdd.end())
					return find->second.get();
			}

			return nullptr;
		}
		
		std::unordered_map<size_t, std::unique_ptr<Component>> m_Components = {};
		std::unordered_map<size_t, std::unique_ptr<Component>> m_PendingComponentsToAdd = {};

	private:
		// Make new private, object must be initialized through World
		void* operator new (std::size_t sz)
		{
			void* p = nullptr;

			/* malloc (0) is unpredictable; avoid it.  */
			if (sz == 0)
				sz = 1;

			while (__builtin_expect((p = malloc(sz)) == 0, false))
			{
				std::new_handler handler = std::get_new_handler();

				if (!handler)
					throw std::bad_alloc{};

				handler();
			}

			return p;
		}
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
		World& GetWorld() const { return m_Entity.GetWorld(); }

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

		(void)Add;
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

				if (GetEntity().m_PendingComponentsToAdd.find(ID) != GetEntity().m_PendingComponentsToAdd.end())
					return nullptr;

				m_DependencyTypes.push_back(ID);

				if (GetEntity().template GetComponent<DependencyTypes>())
					return nullptr;

				GetEntity().m_PendingComponentsToAdd.emplace(
					std::pair<size_t, std::unique_ptr<Component>>(
						ID,
						nullptr
					)
				);

				std::unique_ptr<DependencyTypes> Comp = std::make_unique<DependencyTypes>(m_Entity);
				Comp->m_ID = ID;
				GetEntity().m_PendingComponentsToAdd[ID] = std::move(Comp);

				return nullptr;
			}() ...
		};

		(void)Add;
	}
	
	template <class T>
	T* Entity::GetComponent(bool SearchPending)
	{
		static_assert(
			std::is_base_of<Component, T>::value,
			"Type is not a Component"
		);
		
		return static_cast<T*>(GetComponentByID(
			Component::GetClassID<T>(), SearchPending
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
		for (auto& Pair : m_PendingComponentsToAdd)
		{
			if (!Pair.second || GetComponentByID(Pair.second->GetID(), false))
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
			m_PendingComponentsToAdd.clear();
			return nullptr;
		}
		
		for (auto& Pair : m_PendingComponentsToAdd)
		{
			if (!Pair.second)
				continue;
		
			if (!GetComponentByID(Pair.second->GetID(), false))
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
		
		m_PendingComponentsToAdd.clear();
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
