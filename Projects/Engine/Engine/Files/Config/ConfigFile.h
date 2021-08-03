#pragma once

#include "../../../../../Libraries/mINI/ini.h"
#include "Engine/Files/FileReference.h"

namespace Engine
{
	class Engine_API ConfigFile : public FileReference
	{
	public:
		ConfigFile(const String& filePath) : FileReference(filePath)
		{
			m_File = mINI::INIFile(filePath);
			m_File.read(m_Structure);
		};

		String GetValue(const String& section, const String& key) const
		{
			return m_Structure.get(section).get(key);
		}

	protected:
		mINI::INIFile m_File;
		mINI::INIStructure m_Structure;
	};
}