#pragma once

#include <vector>
#include <string>

#include "Engine/Core.h"

#ifdef CB_PLATFORM_WINDOWS
typedef int mode_t;
#endif

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
		static char* Read(const String& filePath, uint32_t& length, Type type = E_CONTENT, bool addNull = false);
		static bool Write(const String& filePath, const String& fileName, char* buffer, uint32_t length, Type type = E_CONTENT);
		static bool CreateRecursivePath(const String& Path, Type type = E_CONTENT, mode_t Mode = 0700);
		
		static bool Exists(const String& filePath, Type type = E_CONTENT);

		static String GetPath(const String& filePath, Type type);
		static String GetExtension(const String& filePath);

		static String GetDefaultSeperator() { return m_DefaultSeperator; }

	private:
		static char* ReadStream(const String& filePath, uint32_t& length, Type type, bool addNull = false);

		static std::unordered_map<Type, String> m_WorkingDirectory;
		static String m_DefaultSeperator;
	};
}