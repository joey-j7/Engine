#pragma once

#include "Engine/Objects/Object.h"

#include <vector>

#include "Engine/Objects/Worlds/Entities/Entity.h"

namespace Engine
{
	class Engine_API Component : public Object
	{
		friend class Entity;
		
	public:
		Entity& GetEntity() const { return m_Entity; }
		bool HasForcedUniqueness() const { return m_ForceUniqueness; }
		
		bool IsCompatible(Component* Comp) const
		{
			return std::find(m_ProhibitedTypes.begin(), m_ProhibitedTypes.end(), Comp->GetID()) == m_ProhibitedTypes.end();
		}

		size_t GetID() const { return m_ID; }

	protected:
		Component(Entity& Entity, const std::string& sName = "Component");
		
		Entity& m_Entity;
		bool m_ForceUniqueness = false;
		
		template <typename... ProhibitedTypes>
		constexpr void AddProhibitedTypes()
		{
			m_ProhibitedTypes.reserve(m_ProhibitedTypes.size() + sizeof...(ProhibitedTypes));
			const void* Add[sizeof...(ProhibitedTypes)] = {
				[&]()->void*
				{
					static_assert(
						std::is_base_of<Component, ProhibitedTypes>::value,
						"Type is not a Component"
					);
					
					const size_t ID = m_Entity.GetComponentID<ProhibitedTypes>();

					if (std::find(m_ProhibitedTypes.begin(), m_ProhibitedTypes.end(), ID) != m_ProhibitedTypes.end())
						return nullptr;

					m_ProhibitedTypes.push_back(ID);
					return nullptr;
				}() ...
			};
		}
		
		template <typename... DependencyTypes>
		constexpr void AddDependencyTypes()
		{
			m_DependencyTypes.reserve(m_DependencyTypes.size() + sizeof...(DependencyTypes));
			const void* Add[sizeof...(DependencyTypes)] = {
				[&]()->void*
				{
					static_assert(
						std::is_base_of<Component, DependencyTypes>::value,
						"Type is not a Component"
					);
					
					const size_t ID = m_Entity.GetComponentID<DependencyTypes>();
					
					if (m_Entity.m_PendingComponentsToAdd.find(ID) != m_Entity.m_PendingComponentsToAdd.end())
						return nullptr;

					m_DependencyTypes.push_back(ID);

					if (!m_Entity.GetComponent<DependencyTypes>())
					{
						m_Entity.m_PendingComponentsToAdd.emplace(std::pair<size_t, Component*>(
							ID,
							nullptr
						));
						
						Component* Comp = new DependencyTypes(m_Entity);
						Comp->m_ID = ID;
						m_Entity.m_PendingComponentsToAdd[ID] = Comp;
					}
					
					return nullptr;
				}() ...
			};
		}
		
	private:
		std::vector<size_t> m_DependencyTypes;
		std::vector<size_t> m_ProhibitedTypes;

		size_t m_ID = 0;
	};
}
