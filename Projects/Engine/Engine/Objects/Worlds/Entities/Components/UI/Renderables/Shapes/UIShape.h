#pragma once

#include "Engine/Objects/Worlds/Entities/Components/UI/Renderables/UIRenderable.h"

namespace Engine
{
	class Engine_API UIShape : public UIRenderable
	{
	public:
		UIShape(Entity& Entity, const String& sName = "Shape") : UIRenderable(Entity, sName)
		{

		}
	};
}
