#pragma once

#include "Engine/Objects/Worlds/Entities/Components/UI/Layouts/UILayout.h"

namespace Engine
{
	class Engine_API UICanvasPanel : public UILayout
	{
	public:
		UICanvasPanel(Entity& Entity, const String& sName = "Canvas Panel") : UILayout(Entity, sName) {}\
	};

	Engine_REGISTER_COMP(UICanvasPanel);
}
