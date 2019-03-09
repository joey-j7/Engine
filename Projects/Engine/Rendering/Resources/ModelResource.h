#pragma once

#include "Rendering/Resources/RenderResource.h"

namespace Engine
{
	class Engine_API ModelResource : public RenderResource
	{
	public:
		ModelResource(const std::string& filePath) : RenderResource(filePath)
		{
		}
	};
}