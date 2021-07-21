#pragma once

#include "Engine/UI/Elements/UIEntity.h"

namespace Engine
{
	class Engine_API UIButton : public UIEntity
	{
	public:
		UIButton(const std::string& sName = "Unnamed Button") : UIEntity(sName) {}

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
