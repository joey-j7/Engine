#pragma once

#include "Engine/UI/Elements/UIComponent.h"

namespace Engine
{
	class Engine_API UIButton : public UIComponent
	{
	public:
		UIButton(Entity& Entity, const std::string& sName = "Button") : UIComponent(Entity, sName) {}

	private:
		virtual void OnEnter();
		virtual void OnExit();

		virtual void OnPressed();
		virtual void OnReleased();

		virtual void OnClicked();

		bool IsHovered = false;
		bool IsPressed = false;
	};
}
