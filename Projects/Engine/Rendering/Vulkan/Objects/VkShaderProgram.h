#pragma once

#include "Rendering/ShaderProgram.h"
#include <vector>

#include <vulkan/vulkan.h>

namespace Engine
{
	class VkShaderFile;

	class VkShaderProgram : public ShaderProgram
	{
	public:
		VkShaderProgram(const ShaderProgram::Descriptor& descriptor);
		virtual ~VkShaderProgram();

		virtual void Bind() override;
		virtual void Unbind() override;

		const std::vector<VkPipelineShaderStageCreateInfo>& GetCreateInfo() const { return m_CreateInfo; }

	private:
		std::vector<VkPipelineShaderStageCreateInfo> m_CreateInfo;
		std::vector<VkShaderFile*> m_Resources;
	};
}
