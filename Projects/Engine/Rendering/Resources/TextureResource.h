#pragma once

#include "Rendering/Resources/RenderResource.h"

class TextureResource : public RenderResource
{
public:
	TextureResource(const std::string& filePath) : RenderResource(filePath)
	{
		m_SupportedExtensions = { "png", "jpg" };
	};
};