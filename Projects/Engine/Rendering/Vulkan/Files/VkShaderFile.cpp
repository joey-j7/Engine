#include "pch.h"

#include "VkShaderFile.h"
#include "../VkRenderAPI.h"

#include "Platform/FileLoader.h"

namespace Engine
{
	VkShaderFile::VkShaderFile(const std::string& filePath) : ShaderFile(filePath)
	{
		uint32_t length = 0;
		const char* data = FileLoader::Read(filePath, length, FileLoader::E_ROOT, false);

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = length;
		createInfo.pCode = reinterpret_cast<const uint32_t*>(data);

		if (vkCreateShaderModule(VkRenderAPI::Get().Device, &createInfo, nullptr, &m_ShaderModule) != VK_SUCCESS)
		{
			CB_CORE_WARN("Could not create shader module");
		}

		delete[] data;
	}

	VkShaderFile::~VkShaderFile()
	{
		vkDestroyShaderModule(VkRenderAPI::Get().Device, m_ShaderModule, nullptr);
	}
}