#include "pch.h"

#include "VkShaderProgram.h"

#include "../Files/VkShaderFile.h"
#include "../VkRenderAPI.h"

namespace Engine
{
	VkShaderProgram::VkShaderProgram(const ShaderProgram::Descriptor& descriptor) : ShaderProgram(descriptor)
	{
		auto& api = VkRenderAPI::Get();

		VkPipelineShaderStageCreateInfo shaderStageInfo = {};
		shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageInfo.pName = "main";

		VkShaderFile* pResource = nullptr;

		#define LOAD_SHADER(x, y) \
		{ \
			pResource = static_cast<VkShaderFile*>(api.Load(x)); \
			m_Resources.push_back(pResource); \
			shaderStageInfo.module = pResource->GetModule(); \
			shaderStageInfo.stage = y; \
			m_CreateInfo.push_back(shaderStageInfo); \
		}

		if (!descriptor.Compute.empty())
			LOAD_SHADER(descriptor.Compute + ".vspv", VK_SHADER_STAGE_COMPUTE_BIT)
		if (!descriptor.Pixel.empty())
			LOAD_SHADER(descriptor.Pixel + ".vspv", VK_SHADER_STAGE_FRAGMENT_BIT)
		if (!descriptor.Geometry.empty())
			LOAD_SHADER(descriptor.Geometry + ".vspv", VK_SHADER_STAGE_GEOMETRY_BIT)
		if (!descriptor.TessControl.empty())
			LOAD_SHADER(descriptor.TessControl + ".vspv", VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT)
		if (!descriptor.TessEvaluation.empty())
			LOAD_SHADER(descriptor.TessEvaluation + ".vspv", VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT)
		if (!descriptor.Vertex.empty())
			LOAD_SHADER(descriptor.Vertex + ".vspv", VK_SHADER_STAGE_VERTEX_BIT)

		CB_CORE_INFO("Succesfully created shader program");
	}

	VkShaderProgram::~VkShaderProgram()
	{
		auto& api = VkRenderAPI::Get();

		for (VkShaderFile* pResource : m_Resources)
		{
			api.Unload(pResource);
		}
	}

	void VkShaderProgram::Bind()
	{
	}

	void VkShaderProgram::Unbind()
	{
	}
}
