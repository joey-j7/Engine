#pragma once

#include "Engine/Objects/Worlds/Entities/Components/Component.h"

namespace Engine
{
	class Transform2DComponent;
	
	class Engine_API UIComponent : public Component
	{
	public:
		UIComponent(Entity& Entity, const std::string& sName = "UI Component");
		
	protected:
		Vector4 Padding;
	};
}
