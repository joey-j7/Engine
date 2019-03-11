#include "pch.h"

#ifdef CB_PLATFORM_WINDOWS
#include "Platform/FileLoader.h"

namespace Engine
{
	void FileLoader::Init()
	{
		char workDir[_MAX_PATH];
		char* res = _fullpath(workDir, ".\\", _MAX_PATH);

		if (res == NULL)
			CB_CORE_ERROR("Could not find working directory!");

		m_WorkingDirectory[E_INTERNAL] = workDir;
		m_WorkingDirectory[E_EXTERNAL] = workDir;
		m_WorkingDirectory[E_CONTENT] = std::string(workDir) + "Content\\";

		m_DefaultSeperator = "\\";
	}

	char* FileLoader::Read(const std::string& filePath, Type type, bool addNull)
	{
		return ReadStream(filePath, type, addNull);
	}
}

#endif