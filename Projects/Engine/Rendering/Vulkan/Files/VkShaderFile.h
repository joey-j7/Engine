#pragma once

#include "Engine/Files/Rendering/ShaderFile.h"
#include <vulkan/vulkan.h>

namespace Engine
{
	class Engine_API VkShaderFile : public ShaderFile
	{
	public:
		VkShaderFile(const std::string& filePath);
		virtual ~VkShaderFile();

		const VkShaderModule& GetModule() const { return m_ShaderModule; };

	private:
		VkShaderModule m_ShaderModule = {};
	};
}