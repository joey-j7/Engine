#pragma once

#include "Rendering/Passes/DrawPass.h"
#include <vulkan/vulkan.h>

#include <vector>

namespace Engine
{
	class VkCommandEngine;

	class Engine_API VkDrawPass : public DrawPass
	{
	public:
		VkDrawPass(VkCommandEngine& engine, const ShaderProgram::Descriptor& shaderPipeline);
		virtual ~VkDrawPass();

		virtual bool Begin() override;
		virtual bool Draw() override;
		virtual bool End() override;

	private:
		VkRenderPass m_RenderPass;

		VkClearValue m_ClearValue;
		VkRenderPassBeginInfo m_RenderPassBeginInfo = {};

		VkPipeline m_Pipeline;
		VkPipelineLayout m_PipelineLayout;

		VkCommandEngine* m_pCommandEngine = nullptr;

		std::vector<VkFramebuffer> m_Framebuffers;
	};
}
