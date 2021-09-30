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

	String FileLoader::GetAbsolutePath(const String& filePath, Type type)
	{
		String path = filePath;
		ReplaceAll(path, "/", m_DefaultSeperator);

		if (!m_WorkingDirectory[type].empty())
			path = m_WorkingDirectory[type] + path;

		return path;
	}

	String FileLoader::GetPath(const String& filePath)
	{
		size_t i = filePath.rfind('/', filePath.length());

		if (i != String::npos)
		{
			return(filePath.substr(0, i + 1));
		}

		return(filePath);
	}

	String FileLoader::GetName(const String& filePath)
	{
		size_t i = filePath.rfind('.', filePath.length());

		if (i != String::npos)
		{
			size_t j = filePath.rfind('/', filePath.length()) + 1;

			if (j == String::npos)
				j = 0;

			return(filePath.substr(j, i - j));
		}

		return("");
	}

	String FileLoader::GetExtension(const String& filePath)
	{
		size_t i = filePath.rfind('.', filePath.length());
		
		if (i != String::npos)
		{
			return filePath.substr(i + 1);
		}

		return("");
	}

	char* FileLoader::ReadStream(const String& filePath, uint32_t& length, Type type, bool addNull)
	{
		// Retrieve full absolute path
		String path = GetAbsolutePath(filePath, type);

		auto ifs = std::ifstream(filePath.c_str(), std::ios::in | std::ios::binary);
		
		if (ifs.is_open())
		{
			std::ifstream::pos_type pos = ifs.tellg();

			ifs.seekg(0, std::ios::end);
			length = static_cast<uint32_t>(ifs.tellg());
			char* result = new char[addNull ? length + 1 : length];

			ifs.seekg(0, std::ios::beg);
			ifs.read(result, length);
			ifs.close();

			if (addNull)
				result[length] = '\0';

			CB_CORE_INFO("Loaded file at path \"{0}\"", path);
			return result;
		}

		CB_CORE_ERROR("Could not open file at path \"{0}\"!", path);
		return nullptr;
	}
	
	bool FileLoader::Write(const String& filePath, const String& fileName, char* buffer, uint32_t length, bool overwrite, Type type)
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

		if (!overwrite)
		{
			int32_t i = 0;
			String fileNameNoExt = GetName(fileName);
			String extension = GetExtension(fileName);

			while (true)
			{
				String P = i == 0 ?
					(FullPath + fileName).c_str() :
					(FullPath + fileNameNoExt + "_" + std::to_string(i) + "." + extension)
				;

				std::ifstream f(P);

				// If it already exists, change Full filepath
				if (f.good())
				{
					i++;
					continue;
				}

				if (i > 0)
					FullFilePath = P;

				break;
			}
		}

		auto file = std::ofstream(FullFilePath.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
		file.write(buffer, length);
		file.close();

		if (!file)
		{
			CB_CORE_ERROR("Could not create file at path \"{0}\"!", FullFilePath);
			return false;
		}

		return true;
	}

	bool FileLoader::Delete(const String& filePath, const String& fileName, Type type)
	{
		// Retrieve full absolute path
		String path = GetAbsolutePath(filePath + fileName, type);
		bool Succeeded = std::remove(path.c_str()) == 0;

		if (!Succeeded)
		{
			CB_CORE_ERROR("Could not delete file at path \"{0}\"!", path);
		}

		return Succeeded;
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