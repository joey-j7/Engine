#pragma once

#include "Engine/Objects/Worlds/Entities/Components/UI/Elements/UIElement.h"

namespace Engine
{
	class Engine_API UIShape : public UIElement
	{
	public:
		UIShape(Entity& Entity, const String& sName = "Shape") : UIElement(Entity, sName)
		{

		}
	};
}
