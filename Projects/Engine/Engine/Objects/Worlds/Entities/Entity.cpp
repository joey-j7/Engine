#include "pch.h"
#include "Entity.h"

namespace Engine
{
	Entity::~Entity()
	{
		SetParent(nullptr);

		for (auto& Component : m_Components)
		{
			delete Component.second;
		}
	}

	Component* Entity::AddComponent(Component* Comp)
	{
		bool Prohibited = false;

		// Check for prohibited components for dependency components
		for (auto& Pair : Comp->m_PendingComponentsToAdd)
		{
			if (!Pair.second || GetComponentByID(Pair.second->GetID()))
				continue;

			if (!Pair.second->IsCompatible(Comp) || !Comp->IsCompatible(Pair.second))
			{
				CB_CORE_ERROR(
					"Cannot add component {0} to entity {1} because of conflicting component {2}!",
					Comp->GetName(), GetName(), Pair.second->GetName()
				);

				Prohibited = true;
			}

			for (auto& CompPair : m_Components)
			{
				if (!Pair.second->IsCompatible(CompPair.second) || !CompPair.second->IsCompatible(Pair.second))
				{
					CB_CORE_ERROR(
						"Cannot add component {0} to entity {1} because of conflicting component {2}!",
						Pair.second->GetName(), GetName(), CompPair.second->GetName()
					);

					Prohibited = true;
				}
			}
		}

		// Check for prohibited components for main component
		for (auto& Pair : m_Components)
		{
			if (!Comp->IsCompatible(Pair.second) || !Pair.second->IsCompatible(Comp))
			{
				CB_CORE_ERROR(
					"Cannot add component {0} to entity {1} because of conflicting component {2}!",
					Comp->GetName(), GetName(), Pair.second->GetName()
				);

				Prohibited = true;
			}
		}

		if (Prohibited)
		{
			// for (auto Pair : Comp->m_PendingComponentsToAdd)
			// {
			// 	if (Pair.second)
			// 		delete Pair.second;
			// }

			Comp->m_PendingComponentsToAdd.clear();
			return nullptr;
		}

		// Now add component
		m_Components.insert(std::pair<size_t, Component*>(Comp->m_ID, Comp));

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
				m_Components.insert(Pair);
			}
			// else
			//	delete Pair.second;
		}

		Comp->m_PendingComponentsToAdd.clear();
		OnComponentAdded(*Comp);

		return Comp;
	}

	bool Entity::RemoveComponent(Component* Comp)
	{
		auto Find = m_Components.find(Comp->GetID());
		const bool Found = Find != m_Components.end();

		if (Found)
		{
			if (Find->second)
				OnComponentRemoved(*Find->second);

			m_Components.erase(Find);
		}

		return Found;
	}

	void Entity::SetParent(Entity* NewEntity)
	{
		if (Parent == NewEntity)
			return;

		if (Parent)
			Parent->Children.remove(this);

		Entity* OldEntity = Parent;
		Parent = NewEntity;
		
		OnParentChanged(OldEntity, NewEntity);

		if (!Parent)
			return;

		Parent->Children.push_back(this);
	}
}
