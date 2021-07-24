#pragma once

#include "Component.h"

#include "Engine/Objects/Renderable.h"
#include "Transform/Transform.h"

namespace Engine
{
	class Engine_API RenderComponent : public Component, public Renderable
	{
	public:
		RenderComponent(Entity& Entity, const std::string& sName = "Button");

	private:
		void OnComponentAdded(Component& Component);
		void OnComponentRemoved(Component& Component);

		template<typename T, typename R>
		void OnTransformation(const Transform<T, R>& Old, const Transform<T, R>& New)
		{
			MarkDirty();
		}
	};
}
