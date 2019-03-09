#pragma once

#include "Rendering/Resources/RenderResource.h"

namespace Engine
{
	class Engine_API TextureResource : public RenderResource
	{
	public:
		TextureResource(const std::string& filePath) : RenderResource(filePath)
		{
		};
	};
}