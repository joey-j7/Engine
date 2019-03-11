#include "pch.h"
#include "ResourceType.h"

#include "Platform/FileLoader.h"

namespace Engine
{
	ResourceType::ResourceType(const std::string& filePath)
	{
		m_FilePath = filePath;
	}
}