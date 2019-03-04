#pragma once

#include "Rendering/Resources/RenderResource.h"

class ShaderResource : public RenderResource
{
public:
	ShaderResource(const std::string& filePath) : RenderResource(filePath) {};
};