#include "pch.h"
#include "UIGridPanel.h"

namespace Engine
{
	void UIGridPanel::SetSlotCount(UVector2& Count)
	{
		if (SlotCount == Count)
			return;

		SlotCount = Count;
		// MarkDirty();
	}
}