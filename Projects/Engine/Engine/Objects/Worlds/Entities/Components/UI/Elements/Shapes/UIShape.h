#pragma once

#include "Engine/Objects/Worlds/Entities/Components/UI/Elements/UIElement.h"

namespace Engine
{
	class Engine_API UIShape : public UIElement
	{
	public:
		UIShape(Entity& Entity, const std::string& sName = "Text") : UIElement(Entity, sName)
		{

		}
	};
}
