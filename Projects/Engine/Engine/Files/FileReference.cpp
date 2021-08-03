#include "pch.h"
#include "FileReference.h"

#include "Platform/FileLoader.h"

namespace Engine
{
	FileReference::FileReference(const String& filePath) : Object(filePath)
	{
		m_FilePath = filePath;
		m_Extension = FileLoader::GetExtension(filePath);
	}
}