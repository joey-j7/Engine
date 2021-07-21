#include "pch.h"
#include "UIButton.h"


namespace Engine
{
	void UIButton::OnEnter()
	{
		IsHovered = true;
	}

	void UIButton::OnExit()
	{
		IsHovered = false;
	}

	void UIButton::OnPressed()
	{
		IsPressed = true;
	}

	void UIButton::OnReleased()
	{
		if (IsHovered)
		{
			OnClicked();
		}
		
		IsPressed = false;
	}

	void UIButton::OnClicked()
	{
		
	}
}
