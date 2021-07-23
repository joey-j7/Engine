#pragma once

#include "Engine/Objects/Worlds/Entities/Components/UI/UIComponent.h"

namespace Engine
{
	class Engine_API UIShape : public UIComponent
	{
	public:
		UIShape(Entity& Entity, const std::string& sName = "Text") : UIComponent(Entity, sName)
		{

		}
	};
}
