#pragma once

#include "Engine/UI/Elements/UIEntity.h"

namespace Engine
{
	class Engine_API UICanvasPanel : public UIEntity
	{
	public:
		UICanvasPanel(const std::string& sName = "Unnamed Canvas Panel") : UIEntity(sName) {}
	};
}
