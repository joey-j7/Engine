#include "pch.h"
#include "UIEntity.h"

#include "Engine/Objects/Worlds/Entities/Components/Transform/Transform2DComponent.h"
#include "Engine/Objects/Worlds/Entities/Components/Transform/Transform3DComponent.h"

namespace Engine
{
	UIEntity::UIEntity(const std::string& sName) : StaticEntity(sName)
	{
		AddComponent<Transform2DComponent>();
		AddComponent<Transform3DComponent>();
	}

	Transform2DComponent& UIEntity::GetTransform()
	{
		return *GetComponent<Transform2DComponent>();
	}
}
