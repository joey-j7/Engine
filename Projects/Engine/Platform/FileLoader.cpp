#include "pch.h"

#include "FileLoader.h"
#include <sys/stat.h>

namespace Engine {
	std::string FileLoader::m_DefaultSeperator = "/";

	std::string FileLoader::GetPath(const std::string& filePath, Type type)
	{
		std::string path = filePath;
		return m_WorkingDirectory[type] + ReplaceAll(path, "/", m_DefaultSeperator);
	}

	std::string FileLoader::GetExtension(const std::string& filePath)
	{
		size_t i = filePath.rfind('.', filePath.length());
		
		if (i != std::string::npos)
		{
			return(filePath.substr(i + 1, filePath.length() - i));
		}

		return("");
	}

	char* FileLoader::ReadStream(const std::string& filePath, uint32_t& length, Type type, bool addNull)
	{
		// Retrieve full absolute path
		std::string path = GetPath(filePath, type);

		std::ifstream ifs(path.c_str(), std::ios::binary | std::ios::ate);

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

	// Define root as empty string
	std::unordered_map<FileLoader::Type, std::string> FileLoader::m_WorkingDirectory = {
		std::pair<FileLoader::Type, std::string>(E_ROOT, "")
	};
}