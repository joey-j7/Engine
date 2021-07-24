#pragma once

#include "Engine/Objects/Object.h"
#include "Engine/Objects/Worlds/Entities/Components/RegisterComponent.h"

#include <vector>

namespace Engine
{
	class Entity;
	
	class Engine_API Component : public Object
	{
		friend class Entity;
		
	public:
		Component(Entity& Entity, const std::string& sName = "Component");
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

		template <class T>
		static size_t GetClassID();

		size_t GetID() const { return m_ID; }
		
	protected:
		Entity& m_Entity;
		bool m_ForceUniqueness = false;
		
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
}
