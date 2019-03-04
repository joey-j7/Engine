#pragma once

#include "Engine/Resources/ResourceType.h"

class RenderResource : public ResourceType
{
public:
	RenderResource(const std::string& filePath) : ResourceType(filePath) {};

protected:
	uint32_t m_Handle = -1;
};