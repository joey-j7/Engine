#pragma once

#include "Engine/Objects/Worlds/Entities/StaticEntity.h"

namespace Engine
{
	class Transform2DComponent;
	
	class Engine_API UIEntity : public StaticEntity
	{
	public:
		UIEntity(const std::string& sName = "Unnamed UI Entity");
		
		Transform2DComponent& GetTransform();
		
	protected:
		Vector4 Padding;
	};
}
