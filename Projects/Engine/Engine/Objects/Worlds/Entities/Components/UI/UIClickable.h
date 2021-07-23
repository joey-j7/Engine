#pragma once

#include "UIComponent.h"

namespace Engine
{
	class Engine_API UIClickable : public UIComponent
	{
	public:
		UIClickable(Entity& Entity, const std::string& sName = "Button") : UIComponent(Entity, sName) {}

		virtual void Draw() override {}
		
	private:
		virtual void OnEnter();
		virtual void OnExit();

		virtual void OnPressed();
		virtual void OnReleased();

		virtual void OnClicked();

		bool IsHovered = false;
		bool IsPressed = false;
	};
	
	Engine_REGISTER_COMP(UIClickable);
}
