#pragma once

#include <vector>
#include <string>

#include "Engine/Core.h"

namespace Engine
{
	class Engine_API FileLoader
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
		static char* Read(const std::string& filePath, uint32_t& length, Type type = E_CONTENT, bool addNull = false);
		static bool Write(const std::string& filePath, const std::string& fileName, char* buffer, uint32_t length, Type type = E_CONTENT);
		static bool CreateRecursivePath(const std::string& Path, Type type = E_CONTENT, mode_t Mode = 0700);
		
		static bool Exists(const std::string& filePath, Type type = E_CONTENT);

		static std::string GetPath(const std::string& filePath, Type type);
		static std::string GetExtension(const std::string& filePath);

		static std::string GetDefaultSeperator() { return m_DefaultSeperator; }

	private:
		static char* ReadStream(const std::string& filePath, uint32_t& length, Type type, bool addNull = false);

		static std::unordered_map<Type, std::string> m_WorkingDirectory;
		static std::string m_DefaultSeperator;
	};
}