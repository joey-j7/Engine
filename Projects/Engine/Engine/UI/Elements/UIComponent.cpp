#include "pch.h"
#include "UIComponent.h"

#include "Engine/Objects/Worlds/Entities/Components/Transform/Transform2DComponent.h"

namespace Engine
{
	UIComponent::UIComponent(Entity& Entity, const std::string& sName) : Component(Entity, sName)
	{
		AddDependencyTypes<Transform2DComponent>();
	}
}
