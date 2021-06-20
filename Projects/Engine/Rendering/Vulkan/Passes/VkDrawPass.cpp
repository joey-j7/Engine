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

		API = &VkRenderAPI::Get();

		Init();
	}

	VkDrawPass::~VkDrawPass()
	{
		Deinit();
	}

	void VkDrawPass::Init()
	{
		if (m_Initialized)
			return;

		CreateRenderPass();
		CreateGraphicsPipeline();
		CreateFramebuffers();
		CreateCommandBuffers();

		m_Initialized = true;
	}

	void VkDrawPass::Deinit()
	{
		if (!m_Initialized)
			return;

		for (auto& framebuffer : m_Framebuffers)
		{
			vk(DestroyFramebuffer, API->Device, framebuffer, nullptr);
		}

		vk(
			FreeCommandBuffers,
			API->Device,
			m_pCommandEngine->GetCommandPool(),
			static_cast<uint32_t>(
				m_CommandBuffers.size()
			),
			m_CommandBuffers.data()
		);

		m_Framebuffers.clear();
		m_CommandBuffers.clear();

		vk(DestroyPipeline, API->Device, m_Pipeline, nullptr);
		vk(DestroyPipelineLayout, API->Device, m_PipelineLayout, nullptr);
		vk(DestroyRenderPass, API->Device, m_RenderPass, nullptr);

		m_Initialized = false;
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

		return true;
	}

	bool VkDrawPass::Draw()
	{
		if (!DrawPass::Draw())
			return false;

		const VkCommandBuffer& commandBuffer = GetCommandBuffer();

		vk(CmdBindPipeline, commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
		vk(CmdDraw, commandBuffer, m_uiVertexCount, m_uiInstanceCount, 0, 0);

		return true;
	}

	bool VkDrawPass::End()
	{
		if (!DrawPass::End())
			return false;

		const VkCommandBuffer& commandBuffer = GetCommandBuffer();
		vk(CmdEndRenderPass, commandBuffer);

		return true;
	}

	const VkCommandBuffer& VkDrawPass::GetCommandBuffer() const
	{
		return m_CommandBuffers[VkRenderAPI::Get().SwapchainCtx.FrameIndex];
	}

	void VkDrawPass::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = API->SwapchainCtx.CreateInfo.imageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		auto res = vk(CreateRenderPass, API->Device, &renderPassInfo, nullptr, &m_RenderPass);
		API->Verify(res);
	}

	void VkDrawPass::CreateGraphicsPipeline()
	{
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float) API->SwapchainCtx.Extent.width;
        viewport.height = (float) API->SwapchainCtx.Extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = API->SwapchainCtx.Extent;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

		auto res = vk(CreatePipelineLayout, API->Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout);
		API->Verify(res);

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;

		pipelineInfo.pStages = static_cast<VkShaderProgram*>(
			m_pShaderProgram.get()
		)->GetCreateInfo().data();
		
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

		res = vk(CreateGraphicsPipelines, 
			API->Device,
			VK_NULL_HANDLE,
			1,
			&pipelineInfo,
			nullptr,
			&m_Pipeline
		);
		
		API->Verify(res);
	}

	void VkDrawPass::CreateFramebuffers()
	{
		const auto& swapchain = API->SwapchainCtx;
		auto& views = swapchain.Views;
		
		m_Framebuffers.resize(views.size());

        for (size_t i = 0; i < views.size(); i++) {
            VkImageView attachments[] = {
                views[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_RenderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = swapchain.Extent.width;
            framebufferInfo.height = swapchain.Extent.height;
            framebufferInfo.layers = 1;

			const auto res = vk(CreateFramebuffer, API->Device, &framebufferInfo, nullptr, &m_Framebuffers[i]);
			VkRenderAPI::Verify(res);
        }
	}

	void VkDrawPass::CreateCommandBuffers() {
		m_CommandBuffers.resize(API->SwapchainCtx.Views.size());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_pCommandEngine->GetCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

		auto res = vk(AllocateCommandBuffers, API->Device, &allocInfo, m_CommandBuffers.data());
		VkRenderAPI::Verify(res);

		for (size_t i = 0; i < m_CommandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			res = vk(BeginCommandBuffer, m_CommandBuffers[i], &beginInfo);
			VkRenderAPI::Verify(res);

			VkClearValue clearValue;
			VkRenderPassBeginInfo renderPassBeginInfo = {};

			renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassBeginInfo.renderPass = m_RenderPass;
			renderPassBeginInfo.framebuffer = m_Framebuffers[i];
			renderPassBeginInfo.renderArea.offset = { 0, 0 };
			renderPassBeginInfo.renderArea.extent = API->SwapchainCtx.Extent;

			clearValue = { m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, 1.0f };
			renderPassBeginInfo.clearValueCount = 1;
			renderPassBeginInfo.pClearValues = &clearValue;

			vk(CmdBeginRenderPass, m_CommandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			vk(CmdBindPipeline, m_CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
			vk(CmdDraw, m_CommandBuffers[i], 3, 1, 0, 0);
			vk(CmdEndRenderPass, m_CommandBuffers[i]);

			res = vk(EndCommandBuffer, m_CommandBuffers[i]);
			VkRenderAPI::Verify(res);
		}
	}

	void VkDrawPass::FreeCommandBuffers()
	{
		vk(FreeCommandBuffers, API->Device, m_pCommandEngine->GetCommandPool(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
	}
}
