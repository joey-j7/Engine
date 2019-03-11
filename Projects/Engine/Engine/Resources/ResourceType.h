#pragma once

#include <string>
#include <stdint.h>

namespace Engine
{
	class Engine_API ResourceType
	{
	public:
		ResourceType(const std::string& filePath);

		void Add() { m_ReferenceCount++; }
		void Remove() { if (m_ReferenceCount > 0) m_ReferenceCount--; }

		const bool HasReferences() const { return m_ReferenceCount > 0; }
		const std::string& GetFilePath() const { return m_FilePath; }

	protected:
		std::string m_FilePath = "";
		uint32_t m_ReferenceCount = 0;
	};
}