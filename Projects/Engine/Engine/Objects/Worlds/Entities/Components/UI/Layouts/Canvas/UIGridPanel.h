#pragma once

#include "Engine/Objects/Worlds/Entities/Components/UI/Layouts/UILayout.h"

namespace Engine
{
	class Engine_API UIGridPanel : public UILayout
	{
	public:
		UIGridPanel(Entity& Entity, const String& sName = "Grid Panel") : UILayout(Entity, sName) {}

		void SetSlotCount(UVector2& Count);
		const UVector2& GetSlotCount() const { return SlotCount; }

	protected:
		// 0 = infinite
		UVector2 SlotCount = UVector2(1, 0);
		std::vector<UIElement*> Elements;
	};
	
	Engine_REGISTER_COMP(UIGridPanel);
}
