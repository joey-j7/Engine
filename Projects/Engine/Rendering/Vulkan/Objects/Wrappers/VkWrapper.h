#pragma once

#include "Engine/Objects/Object.h"

#include "Rendering/Vulkan/VkRenderAPI.h"
#include "Rendering/Vulkan/Objects/VkHandle.h"

namespace Engine
{
	class Engine_API VkWrapper : public Object
	{
	public:
		VkWrapper(const std::string& sName = "Vulkan Object");

		bool IsValid() const
		{
			return m_Initialized;
		}
		
	protected:
		VkRenderAPI& m_API;
		bool m_Initialized = false;
	};
}
