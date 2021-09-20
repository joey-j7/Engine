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
		m_WorkingDirectory[E_CONTENT] = String(workDir) + "Content\\";

		m_DefaultSeperator = "\\";
	}

	std::vector<String> FileLoader::GetFilenames(const String& folderPath, const std::vector<String>& extensions, Type type)
	{
		std::vector<String> Filenames;

		// Retrieve full absolute path
		String path = GetPath(folderPath, type);

		if (!std::filesystem::exists(path))
			return Filenames;

		for (const auto& entry : std::filesystem::directory_iterator(path))
		{
			String Extension = entry.path().extension().u8string();
			Extension = Extension.substr(1, Extension.size() - 1);

			if (extensions.empty() ||
				std::find(
					extensions.begin(),
					extensions.end(),
					Extension
				) != extensions.end())
			{
				Filenames.push_back(
					entry.path().filename().u8string()
				);
			}
		}

		return Filenames;
	}

	char* FileLoader::Read(const String& filePath, uint32_t& length, Type type, bool addNull)
	{
		return ReadStream(filePath, length, type, addNull);
	}

	bool FileLoader::Exists(const String& filePath, Type type /*= E_CONTENT*/)
	{
		// Retrieve full absolute path
		String path = GetPath(filePath, type);

		struct stat buffer;
		return (stat(path.c_str(), &buffer) == 0);
	}
}

#endif