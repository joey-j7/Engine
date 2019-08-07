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

	char* FileLoader::Read(const std::string& filePath, uint32_t& length, Type type, bool addNull)
	{
		return ReadStream(filePath, length, type, addNull);
	}

	bool FileLoader::Exists(const std::string& filePath, Type type /*= E_CONTENT*/)
	{
		// Retrieve full absolute path
		std::string path = GetPath(filePath, type);

		struct stat buffer;
		return (stat(path.c_str(), &buffer) == 0);
	}
}

#endif