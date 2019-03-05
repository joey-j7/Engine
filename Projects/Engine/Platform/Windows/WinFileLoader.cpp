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
			CB_CORE_ERROR_T(5.0f, "Could not find working directory!");

		m_WorkingDirectory[E_INTERNAL] = workDir;
		m_WorkingDirectory[E_EXTERNAL] = workDir;
		m_WorkingDirectory[E_CONTENT] = std::string(workDir) + "Content\\";

		m_DefaultSeperator = "\\";
	}

	std::vector<char> FileLoader::Read(std::string filePath, Type type)
	{
		return ReadStream(filePath, type);
	}
}

#endif