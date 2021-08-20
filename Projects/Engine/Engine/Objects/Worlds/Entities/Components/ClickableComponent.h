#pragma once

#include "Engine/Objects/Clickable.h"

namespace Engine
{
	class Engine_API ClickableComponent : public Component, public Clickable
	{
	public:
		ClickableComponent(Entity& Entity, const String& sName = "Clickable");
		
		virtual const AABB GetBounds() const override;
	};
	
	Engine_REGISTER_COMP(ClickableComponent);
}
