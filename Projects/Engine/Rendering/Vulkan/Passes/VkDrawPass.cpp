#include "pch.h"

#include "VkDrawPass.h"

#include "../VkRenderAPI.h"

#include "../Objects/VkShaderProgram.h"
#include "../VkCommandEngine.h"

namespace Engine
{
	VkDrawPass::VkDrawPass(VkCommandEngine& engine, const ShaderProgram::Descriptor& shaderPipeline) : DrawPass(shaderPipeline)
	{
		m_pCommandEngine = &engine;

		VkRenderAPI& api = VkRenderAPI::Get();

		/* Create render pass */
		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = api.SwapchainCtx.ImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		if (vkCreateRenderPass(api.Device, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}

		/* Create pipeline */
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pushConstantRangeCount = 0;

		if (vkCreatePipelineLayout(api.Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)api.SwapchainCtx.Extent.width;
		viewport.height = (float)api.SwapchainCtx.Extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = api.SwapchainCtx.Extent;

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		VkGraphicsPipelineCreateInfo pipelineInfo = {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = static_cast<VkShaderProgram*>(m_pShaderProgram.get())->GetCreateInfo().data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.layout = m_PipelineLayout;
		pipelineInfo.renderPass = m_RenderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		VkResult err = vkCreateGraphicsPipelines(api.Device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline);
		VkRenderAPI::Verify(err);

		/* Setup RenderPass Swap Info */
		m_RenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		m_RenderPassBeginInfo.renderPass = m_RenderPass;

		m_RenderPassBeginInfo.renderArea.offset = { 0, 0 };
		m_RenderPassBeginInfo.renderArea.extent = api.SwapchainCtx.Extent;

		m_ClearValue = { m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, 1.0f };
		m_RenderPassBeginInfo.clearValueCount = 1;
		m_RenderPassBeginInfo.pClearValues = &m_ClearValue;

		/* Setup frame buffer */
		m_Framebuffers.resize(api.SwapchainCtx.Images.size());

		for (size_t i = 0; i < api.SwapchainCtx.Images.size(); i++) {
			VkImageView attachments[] = {
				api.SwapchainCtx.Views[i]
			};

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_RenderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = api.SwapchainCtx.Extent.width;
			framebufferInfo.height = api.SwapchainCtx.Extent.height;
			framebufferInfo.layers = 1;

			err = vkCreateFramebuffer(api.Device, &framebufferInfo, nullptr, &m_Framebuffers[i]);
			api.Verify(err);
		}
	}

	VkDrawPass::~VkDrawPass()
	{
		VkRenderAPI& api = VkRenderAPI::Get();

		vkDestroyPipeline(api.Device, m_Pipeline, nullptr);
		vkDestroyPipelineLayout(api.Device, m_PipelineLayout, nullptr);
		vkDestroyRenderPass(api.Device, m_RenderPass, nullptr);

		for (auto& framebuffer : m_Framebuffers)
		{
			vkDestroyFramebuffer(api.Device, framebuffer, nullptr);
		}
	}

	bool VkDrawPass::Begin()
	{
		if (m_pCommandEngine->GetState() != CommandEngine::E_RECORDING)
		{
			CB_CORE_WARN("Tried to begin a draw pass while the command engine is busy or hasn't been started yet!");
			return false;
		}

		if (!DrawPass::Begin())
			return false;

		const VkCommandBuffer& commandBuffer = m_pCommandEngine->GetCommandBuffer();

		m_RenderPassBeginInfo.framebuffer = m_Framebuffers[VkRenderAPI::Get().SwapchainCtx.FrameIndex];
		vkCmdBeginRenderPass(commandBuffer, &m_RenderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		return true;
	}

	bool VkDrawPass::Draw()
	{
		if (!DrawPass::Draw())
			return false;

		const VkCommandBuffer& commandBuffer = m_pCommandEngine->GetCommandBuffer();

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
		vkCmdDraw(commandBuffer, m_uiVertexCount, m_uiInstanceCount, 0, 0);

		return true;
	}

	bool VkDrawPass::End()
	{
		if (!DrawPass::End())
			return false;

		const VkCommandBuffer& commandBuffer = m_pCommandEngine->GetCommandBuffer();
		vkCmdEndRenderPass(commandBuffer);

		return true;
	}
}
