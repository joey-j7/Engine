#pragma once

#include <string>
#include <stdint.h>

class ResourceType
{
public:
	ResourceType(const std::string& filePath);

protected:
	std::string m_FilePath = "";
	std::vector<std::string> m_SupportedExtensions;

	uint32_t m_ReferenceCount = 0;
};