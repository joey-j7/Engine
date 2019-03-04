#pragma once

#include <string>
#include <stdint.h>

class ResourceType
{
	friend class ResourceManager;

public:
	ResourceType(const std::string& filePath);

protected:
	std::string m_FilePath = "";
	uint32_t m_ReferenceCount = 0;
};