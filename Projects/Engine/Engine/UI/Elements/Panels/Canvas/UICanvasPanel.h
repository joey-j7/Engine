#pragma once

#include "Engine/UI/Elements/UIComponent.h"

namespace Engine
{
	class Engine_API UICanvasPanel : public UIComponent
	{
	public:
		UICanvasPanel(Entity& Entity, const std::string& sName = "Canvas Panel") : UIComponent(Entity, sName) {}
	};
}
