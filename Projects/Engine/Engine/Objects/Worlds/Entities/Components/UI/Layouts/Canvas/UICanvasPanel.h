#pragma once

#include "Engine/Objects/Worlds/Entities/Components/UI/Layouts/UILayout.h"

namespace Engine
{
	class Engine_API UICanvasPanel : public UILayout
	{
	public:
		UICanvasPanel(Entity& Entity, const String& sName = "Canvas Panel") : UILayout(Entity, sName) {}
		
	protected:
		void OnComponentMeasurement(Entity& Entity, ComponentData& ComponentData, uint32_t ComponentID) override;
	};

	Engine_REGISTER_COMP(UICanvasPanel);
}
