#pragma once

#include "Rendering/Resources/RenderResource.h"

class ModelResource : public RenderResource
{
public:
	ModelResource(const std::string& filePath) : RenderResource(filePath)
	{
		m_SupportedExtensions = { "fbx", "obj", "vox" };
	}
};