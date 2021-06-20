#pragma once

#include "Rendering/Passes/DrawPass.h"
#include <vulkan/vulkan.h>

#include <vector>

namespace Engine
{
	class VkRenderAPI;
	class VkCommandEngine;

	class Engine_API VkDrawPass : public DrawPass
	{
	public:
		VkDrawPass(VkCommandEngine& engine, const ShaderProgram::Descriptor& shaderPipeline);
		virtual ~VkDrawPass();

		void Init();
		void Deinit();

		virtual bool Begin() override;
		virtual bool Draw() override;
		virtual bool End() override;

		const VkCommandBuffer& GetCommandBuffer() const;

	private:
		void CreateRenderPass();
		void CreateGraphicsPipeline();
		void CreateFramebuffers();

		void CreateCommandBuffers();
		void FreeCommandBuffers();
		
		VkRenderAPI* API = nullptr;
		
		VkRenderPass m_RenderPass;

		VkPipeline m_Pipeline;
		VkPipelineLayout m_PipelineLayout;

		VkCommandEngine* m_pCommandEngine = nullptr;

		std::vector<VkFramebuffer> m_Framebuffers;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		bool m_Initialized = false;
	};
}
