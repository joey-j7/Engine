#include "pch.h"

#include "VkCommandEngine.h"

#include "VkRenderAPI.h"
#include "Passes/VkDrawPass.h"

namespace Engine
{
	DrawPass* VkCommandEngine::Create(const ShaderProgram::Descriptor& shaderPipeline)
	{
		return new VkDrawPass(*this, shaderPipeline);
	}

	VkCommandEngine::VkCommandEngine(Type type, const std::string& sName) : CommandEngine(type, sName)
	{
		auto& api = VkRenderAPI::Get();

		// Find queue families
		uint32_t graphicsFamily = UINT_MAX;
		uint32_t presentFamily = UINT_MAX;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(api.PhysicalDevice, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(api.PhysicalDevice, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(api.PhysicalDevice, i, api.Surface, &presentSupport);

			if (queueFamily.queueCount > 0 && presentSupport)
			{
				presentFamily = i;
			}

			if (graphicsFamily != UINT_MAX && presentFamily != UINT_MAX)
			{
				break;
			}

			i++;
		}

		/* Create command pool, semaphores, fences */
		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = graphicsFamily == UINT_MAX ? 0 : graphicsFamily;

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		uint32_t uiFrameCount = api.SwapchainCtx.FrameCount;

		m_CommandBuffers.resize(uiFrameCount);

		m_ImageAcquiredSemaphores.resize(uiFrameCount);
		m_RenderCompleteSemaphores.resize(uiFrameCount);
		m_Fences.resize(uiFrameCount);

		VkResult err = vkCreateCommandPool(api.Device, &poolInfo, nullptr, &m_CommandPool);
		VkRenderAPI::Verify(err);

		for (uint32_t i = 0; i < uiFrameCount; ++i)
		{
			allocInfo.commandPool = m_CommandPool;

			err = vkAllocateCommandBuffers(api.Device, &allocInfo, &m_CommandBuffers[i]);
			VkRenderAPI::Verify(err);
		}

		for (uint32_t i = 0; i < uiFrameCount; ++i)
		{
			err = vkCreateSemaphore(api.Device, &semaphoreInfo, nullptr, &m_ImageAcquiredSemaphores[i]);
			VkRenderAPI::Verify(err);

			err = vkCreateSemaphore(api.Device, &semaphoreInfo, nullptr, &m_RenderCompleteSemaphores[i]);
			VkRenderAPI::Verify(err);

			/* Create the fence used to sync command buffer completion */
			err = vkCreateFence(api.Device, &fenceInfo, nullptr, &m_Fences[i]);
			VkRenderAPI::Verify(err);
		}
	}

	VkCommandEngine::~VkCommandEngine()
	{
		// Ensure that the GPU is no longer referencing resources that are about to be
		// cleaned up by the destructor.
		WaitForGPU();

		auto& api = VkRenderAPI::Get();

		for (uint32_t i = 0; i < api.SwapchainCtx.FrameCount; ++i)
		{
			vkDestroyFence(api.Device, m_Fences[i], nullptr);

			vkDestroySemaphore(api.Device, m_RenderCompleteSemaphores[i], nullptr);
			vkDestroySemaphore(api.Device, m_ImageAcquiredSemaphores[i], nullptr);
		}

		vkDestroyCommandPool(api.Device, m_CommandPool, nullptr);
	}

	void VkCommandEngine::Reset()
	{
		if (m_State == E_RECORDING || m_State == E_IDLE)
			return;

		WaitForGPU();

		auto& api = VkRenderAPI::Get();

		// Command list allocators can only be reset when the associated 
		// command lists have finished execution on the GPU; apps should use 
		// fences to determine GPU execution progress.
		VkResult err = vkResetFences(api.Device, 1, &GetFence());
		VkRenderAPI::Verify(err);

		err = vkResetCommandPool(api.Device, GetCommandPool(), 0);
		VkRenderAPI::Verify(err);
	}

	void VkCommandEngine::Begin()
	{
		if (m_State != E_IDLE) return;
		m_State = E_RECORDING;

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		const VkResult err = vkBeginCommandBuffer(GetCommandBuffer(), &beginInfo);
		VkRenderAPI::Verify(err);
	}

	void VkCommandEngine::Execute()
	{
		if (m_State != E_RECORDING) return;
		m_State = E_EXECUTING;

		// Close buffer
		VkResult err = vkEndCommandBuffer(GetCommandBuffer());
		VkRenderAPI::Verify(err);

		// Execute
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore waitSemaphores[] = { GetImageAcquiredSemaphore() };
		VkSemaphore signalSemaphores[] = { GetRenderCompleteSemaphore() };

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.pWaitSemaphores = waitSemaphores;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &GetCommandBuffer();

		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		// Signal
		Signal();

		err = vkQueueSubmit(VkRenderAPI::Get().SwapchainCtx.GraphicsQueue, 1, &submitInfo, GetFence());
		VkRenderAPI::Verify(err);
	}

	void VkCommandEngine::WaitForGPU()
	{
		vkWaitForFences(VkRenderAPI::Get().Device, 1, &GetFence(), VK_TRUE, std::numeric_limits<uint64_t>::max());
		m_State = E_IDLE;
	}

	const VkCommandPool& VkCommandEngine::GetCommandPool() const
	{
		return m_CommandPool;
	}

	const VkCommandBuffer& VkCommandEngine::GetCommandBuffer() const
	{
		return m_CommandBuffers[VkRenderAPI::Get().SwapchainCtx.FrameIndex];
	}

	const VkSemaphore& VkCommandEngine::GetImageAcquiredSemaphore() const
	{
		return m_ImageAcquiredSemaphores[VkRenderAPI::Get().SwapchainCtx.SemaphoreIndex];
	}

	const VkSemaphore& VkCommandEngine::GetRenderCompleteSemaphore() const
	{
		return m_RenderCompleteSemaphores[VkRenderAPI::Get().SwapchainCtx.SemaphoreIndex];
	}

	const VkFence& VkCommandEngine::GetFence() const
	{
		return m_Fences[VkRenderAPI::Get().SwapchainCtx.FrameIndex];
	}

	//void VkCommandEngine::Signal()
	//{
	//	// Increases fence value
	//	CommandEngine::Signal();

	//	if (m_uiFenceValue + 1 > m_Fences.size())
	//	{
	//		VkFenceCreateInfo fenceInfo = {};
	//		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	//		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	//		m_Fences.emplace_back();
	//		VkResult err = vkCreateFence(m_pContextData->Device, &fenceInfo, nullptr, &m_Fences.back());
	//		VkRenderAPI::Verify(err);
	//	}
	//}

	void VkCommandEngine::AdvanceFrame()
	{
		// If the next frame is not ready to be rendered yet, wait until it is ready.
		WaitForGPU();

		// Set the fence value for the next frame.
		m_uiFenceValue = 0;
	}
}