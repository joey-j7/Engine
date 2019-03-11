#pragma once

#include <vector>
#include <string>

#include "Engine/Resources/ResourceType.h"

namespace Engine
{
	class Engine_API RenderResource : public ResourceType
	{
	public:
		RenderResource(const std::string& filePath) : ResourceType(filePath) {};

		uint32_t GetHandle() const { return m_Handle; }
		virtual void SetHandle(uint32_t handle) {
			assert(_CrtCheckMemory());
			m_Handle = handle;
			assert(_CrtCheckMemory());
		}

	protected:
		uint32_t m_Handle = UINT32_MAX;
	};
}