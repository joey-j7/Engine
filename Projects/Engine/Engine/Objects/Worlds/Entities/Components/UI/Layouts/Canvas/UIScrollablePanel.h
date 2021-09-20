#pragma once

#include "UICanvasPanel.h"

namespace Engine
{
	class Engine_API UIScrollablePanel : public UICanvasPanel
	{
	public:
		UIScrollablePanel(Entity& Entity, const String& sName = "Scrollable Panel") : UICanvasPanel(Entity, sName) {}
	};

	Engine_REGISTER_COMP(UIScrollablePanel);
}
