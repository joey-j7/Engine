#include "pch.h"

#include "FileLoader.h"

#include <sys/stat.h>

#include <limits.h>
#include <sys/stat.h> 
#include <errno.h>

#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif

#ifndef MKDIR(x, y)
#ifdef CB_PLATFORM_WINDOWS
#define MKDIR(x, y) _mkdir(x)
#else
#define MKDIR(x, y) mkdir(x, y)
#endif
#endif

namespace Engine {
	String FileLoader::m_DefaultSeperator = "/";

	String FileLoader::GetPath(const String& filePath, Type type)
	{
		String path = filePath;
		ReplaceAll(path, "/", m_DefaultSeperator);

		if (!m_WorkingDirectory[type].empty())
			path = m_WorkingDirectory[type] + path;

		return path;
	}

	String FileLoader::GetExtension(const String& filePath)
	{
		size_t i = filePath.rfind('.', filePath.length());
		
		if (i != String::npos)
		{
			return(filePath.substr(i + 1, filePath.length() - i));
		}

		return("");
	}

	char* FileLoader::ReadStream(const String& filePath, uint32_t& length, Type type, bool addNull)
	{
		// Retrieve full absolute path
		String path = GetPath(filePath, type);

		auto ifs = std::ifstream(filePath.c_str(), std::ios::in | std::ios::binary);
		
		if (ifs.is_open())
		{
			std::ifstream::pos_type pos = ifs.tellg();

			ifs.seekg(0, std::ios::end);
			length = static_cast<uint32_t>(ifs.tellg());
			char* result = new char[addNull ? length + 1 : length];

			ifs.seekg(0, std::ios::beg);
			ifs.read(result, pos);
			ifs.close();

			if (addNull)
				result[length] = '\0';

			CB_CORE_INFO("Loaded file at path \"{0}\"", path);
			return result;
		}

		CB_CORE_ERROR("Could not open file at path \"{0}\"!", path);
		return nullptr;
	}
	
	bool FileLoader::Write(const String& filePath, const String& fileName, char* buffer, uint32_t length, Type type)
	{
		String FullPath = m_WorkingDirectory[type] + filePath;
		String FullFilePath = FullPath + fileName;

		CB_CORE_INFO("Writing file {0} to path {1}", fileName, FullFilePath);
		
		if (!CreateRecursivePath(filePath, type))
		{
			CB_CORE_ERROR("Could not create recursive path \"{0}\"!", FullPath);
			return false;
		}
		
		// mkdir(FullPath.c_str(), 0700);
		
		auto file = std::ofstream(FullFilePath, std::ios::out | std::ios::trunc | std::ios::binary);
		file.write(buffer, length);
		file.close();

		if (!file)
		{
			CB_CORE_ERROR("Could not create file at path \"{0}\"!", FullFilePath);
			return false;
		}

		return true;
	}

	bool FileLoader::CreateRecursivePath(const String& Path, Type type, mode_t Mode)
	{
		String FullPath = m_WorkingDirectory[type] + Path;
		
		const size_t len = FullPath.size();
		char _path[PATH_MAX];
		char* p;

		errno = 0;

		/* Copy string so its mutable */
		if (len > sizeof(_path) - 1) {
			errno = ENAMETOOLONG;
			return false;
		}
		strcpy(_path, FullPath.c_str());

		/* Iterate the string */
		for (p = _path + 1; *p; p++) {
			if (*p == '/') {
				/* Temporarily truncate */
				*p = '\0';

				if (MKDIR(_path, Mode) != 0)
				{
					if (errno != EEXIST)
						return false;
				}

				*p = '/';
			}
		}

		if (MKDIR(_path, Mode) != 0) {
			if (errno != EEXIST)
				return false;
		}

		return true;
	}

	// Define root as empty string
	std::unordered_map<FileLoader::Type, String> FileLoader::m_WorkingDirectory = {
		std::pair<FileLoader::Type, String>(E_ROOT, "")
	};
}