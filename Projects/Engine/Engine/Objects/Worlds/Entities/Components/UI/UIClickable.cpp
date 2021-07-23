#include "pch.h"
#include "UIClickable.h"

namespace Engine
{
	void UIClickable::OnEnter()
	{
		IsHovered = true;
	}

	void UIClickable::OnExit()
	{
		IsHovered = false;
	}

	void UIClickable::OnPressed()
	{
		IsPressed = true;
	}

	void UIClickable::OnReleased()
	{
		if (IsHovered)
		{
			OnClicked();
		}
		
		IsPressed = false;
	}

	void UIClickable::OnClicked()
	{
		
	}
}
