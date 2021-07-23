#pragma once
#include "Engine/Objects/Worlds/Entities/Components/UI/UIComponent.h"

namespace Engine
{
	class Engine_API UICanvasPanel : public UIComponent
	{
	public:
		UICanvasPanel(Entity& Entity, const std::string& sName = "Canvas Panel") : UIComponent(Entity, sName) {}
	};

	Engine_REGISTER_COMP(UICanvasPanel);
}
