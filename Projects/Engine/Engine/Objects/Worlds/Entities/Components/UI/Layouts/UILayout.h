#pragma once

#include "Engine/Objects/Worlds/Entities/Components/UI/Elements/UIElement.h"

namespace Engine
{
	class Engine_API UILayout : public Component
	{
	public:
		struct ComponentData
		{
			ComponentData(UIElement* Comp) : Component(Comp) {}

			bool operator==(const ComponentData& Other) const { return Component == Other.Component; }
			bool operator==(UIElement* Other) const { return Component == Other; }
			
			UIElement* Component = nullptr;
			
			uint32_t IndexX = 0;
			uint32_t IndexY = 0;
		};
		
		UILayout(Entity& Entity, const std::string& sName = "UI Layout");
		virtual ~UILayout() {}

		AABB GetBounds() const;
		
	protected:
		virtual void OnComponentMeasurement(Entity& Entity, ComponentData& ComponentData, uint32_t ComponentID) = 0;
		
		TransformComponent2D* m_Transform = nullptr;

	private:
		void OnChildAdded(Entity& Origin, Entity& Entity);
		void OnChildRemoved(Entity& Origin, Entity& Entity);
		
		void OnComponentAdded(Entity& Origin, Component& Component);
		void OnComponentRemoved(Entity& Origin, Component& Component);

		void OnParentChanged(Entity& Origin, Entity* Old, Entity* New);
		UILayout* CheckParentLayout(Entity& Origin) const;

		std::unordered_map<Entity*, std::vector<ComponentData>> m_EntitiesComponents;
	};
}