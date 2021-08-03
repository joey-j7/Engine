#include "pch.h"
#include "UIOval.h"

namespace Engine
{
	void UIOval::SetWidth(uint32_t Width)
	{
		UIElement::SetWidth(Width);

		uint32_t Max = glm::max(GetWidth(), GetHeight());
		SetRadius(Max);
	}
	
	void UIOval::SetHeight(uint32_t Height)
	{
		UIElement::SetHeight(Height);

		uint32_t Max = glm::max(GetWidth(), GetHeight());
		SetRadius(Max);
	}
}