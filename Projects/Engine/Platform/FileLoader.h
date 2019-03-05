#pragma once

#include <vector>
#include <string>

namespace Engine
{
	class FileLoader
	{
	public:
		enum Type
		{
			E_ROOT,
			E_INTERNAL,
			E_EXTERNAL,
			E_CONTENT
		};

		// Initialize working directory and platform specific variables
		static void Init();

		// Reads the entire file to memory
		static std::vector<char> Read(std::string filePath, Type type = E_CONTENT);

	private:
		static std::vector<char> ReadStream(std::string filePath, Type type);

		static std::unordered_map<Type, std::string> m_WorkingDirectory;
		static std::string m_DefaultSeperator;
	};
}