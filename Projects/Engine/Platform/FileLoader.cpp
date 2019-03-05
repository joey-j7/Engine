#include "pch.h"

#include "FileLoader.h"

namespace Engine {
	std::string FileLoader::m_DefaultSeperator = "/";

	std::vector<char> FileLoader::ReadStream(std::string filePath, Type type)
	{
		// Retrieve full absolute path
		filePath = m_WorkingDirectory[type] + ReplaceAll(filePath, "/", m_DefaultSeperator);

		std::ifstream ifs(filePath.c_str(), std::ios::binary | std::ios::ate);

		if (ifs.is_open())
		{
			std::ifstream::pos_type pos = ifs.tellg();
			std::vector<char> result(pos);

			ifs.seekg(0, std::ios::beg);
			ifs.read(result.data(), pos);
			ifs.close();

			CB_CORE_INFO_T(5.0f, "Loaded file at path {0}", filePath);
			return result;
		}

		CB_CORE_ERROR_T(5.0f, "Could not open file at path {0}!", filePath);
		return {};
	}

	// Define root as empty string
	std::unordered_map<FileLoader::Type, std::string> FileLoader::m_WorkingDirectory = {
		std::pair<FileLoader::Type, std::string>(E_ROOT, "")
	};
}