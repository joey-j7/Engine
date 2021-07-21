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
		
		bool IsCompatible(Component* Component) const
		{
			return std::find(m_ProhibitedTypes.begin(), m_ProhibitedTypes.end(), Component->GetID()) != m_ProhibitedTypes.end();
		}

		uint32_t GetID() const { return m_ID; }

	protected:
		Component(Entity& Entity, const std::string& sName = "Unnamed Component") : Object(sName), m_Entity(Entity) {}
		
		Entity& m_Entity;
		bool m_ForceUniqueness = false;
		
		template <typename... ProhibitedTypes>
		constexpr void AddProhibitedTypes()
		{
			void* Add[sizeof...(ProhibitedTypes)] = {
				[&]()->void*
				{
					const uint32_t ID = m_Entity.GetComponentID<ProhibitedTypes>();

					if (std::find(m_ProhibitedTypes.begin(), m_ProhibitedTypes.end(), ID) != m_ProhibitedTypes.end())
						return nullptr;

					m_ProhibitedTypes.push_back(ID);

					Component* Comp = m_Entity.GetComponent<ProhibitedTypes>();

					if (!Comp)
						Comp = (
							m_Entity.m_PendingComponentsToAdd.find(ID) != m_Entity.m_PendingComponentsToAdd.end() ?
							m_Entity.m_PendingComponentsToAdd.find(ID)->second :
							nullptr
						);

					if (!Comp)
						return nullptr;

					CB_CORE_ERROR(
						"Cannot add component {0} to entity {1} because of conflicting component {2}!",
						GetName(), m_Entity.GetName(), Comp->GetName()
					);

					m_Prohibited = true;
					return nullptr;
				}() ...
			};
		}
		
		template <typename... DependencyTypes>
		constexpr void AddDependencyTypes()
		{
			void* Add[sizeof...(DependencyTypes)] = {
				[&]()->void*
				{
					const uint32_t ID = m_Entity.GetComponentID<DependencyTypes>();
					
					if (m_Entity.m_PendingComponentsToAdd.find(ID) != m_Entity.m_PendingComponentsToAdd.end())
						return nullptr;

					m_DependencyTypes.push_back(ID);

					if (!m_Entity.GetComponent<DependencyTypes>())
					{
						m_Entity.m_PendingComponentsToAdd.emplace(std::pair<uint32_t, Component*>(
							ID,
							nullptr
						));
						
						Component* Comp = new DependencyTypes(m_Entity);
						m_Entity.m_PendingComponentsToAdd[ID] = Comp;

						if (!Comp->IsCompatible(this))
						{
							CB_CORE_ERROR(
								"Cannot add component {0} to entity {1} because of conflicting component {2}!",
								Comp->GetName(), m_Entity.GetName(), GetName()
							);

							m_Prohibited = true;
							return nullptr;
						}
					}
					
					return nullptr;
				}() ...
			};
		}
		
	private:
		std::vector<uint32_t> m_DependencyTypes;
		std::vector<uint32_t> m_ProhibitedTypes;

		uint32_t m_ID = UINT_MAX;
		
		bool m_Prohibited = false;
	};
}
