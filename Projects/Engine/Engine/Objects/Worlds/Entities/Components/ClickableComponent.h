#pragma once

#include "Component.h"
#include "Engine/Objects/Clickable.h"

namespace Engine
{
	class Engine_API ClickableComponent : public Component, public Clickable
	{
	public:
		ClickableComponent(Entity& Entity, const std::string& sName = "Clickable");
		
		virtual const AABB GetBounds() const override;
		
	protected:
		virtual void OnEnter(double XPosition, double YPosition) override;
		virtual void OnExit(double XPosition, double YPosition) override;
		
		virtual void OnPressed() override;
		virtual void OnReleased() override;
	};
	
	Engine_REGISTER_COMP(ClickableComponent);
}
