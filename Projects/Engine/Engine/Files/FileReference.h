#pragma once

#include <string>
#include <stdint.h>

#include "Engine/Objects/Object.h"
#include "Engine/Logger/Log.h"

namespace Engine
{
	class FileManagerBase;

	class Engine_API FileReference : public Object
	{
	public:
		FileReference(const std::string& filePath);

		void Add()
		{
			m_ReferenceCount++;
			CB_CORE_INFO("Resource \"{0}\" reference count incremented to {1}", m_FilePath, m_ReferenceCount);
		}

		void Remove()
		{
			if (m_ReferenceCount > 0) m_ReferenceCount--;
			CB_CORE_INFO("Resource \"{0}\" reference count decremented to {1}", m_FilePath, m_ReferenceCount);
		}

		const bool HasReferences() const { return m_ReferenceCount > 0; }

		const std::string& GetFilePath() const { return m_FilePath; }
		const std::string& GetExtension() const { return m_Extension; }

	protected:
		std::string m_FilePath = "";
		std::string m_Extension = "";

		uint32_t m_ReferenceCount = 0;
	};
}