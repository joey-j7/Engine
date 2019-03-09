#pragma once

#include "Rendering/Resources/RenderResource.h"

namespace Engine
{
	class Engine_API ShaderResource : public RenderResource
	{
	public:
		ShaderResource(const std::string& filePath) : RenderResource(filePath)
		{
		};
	};
}