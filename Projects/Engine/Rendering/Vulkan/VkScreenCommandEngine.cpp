#include "pch.h"
#include "VkScreenCommandEngine.h"

#include "VkRenderAPI.h"

namespace Engine
{
	VkScreenCommandEngine::VkScreenCommandEngine() :
		VkCommandEngine(E_DIRECT, "Screen Command Engine")
	{
		Init();
	}

	void VkScreenCommandEngine::Init()
	{
		VkCommandEngine::Init();

		// Create a direct command buffer for each frame, for render and usage
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT * 2; ++i)
		{
			CreateCommandBuffer();
		}
	}

	bool VkScreenCommandEngine::Swap()
	{
		WaitForGPU();

		VkResult res = vk(
			AcquireNextImageKHR,
			API->Device,
			API->SwapchainCtx.Swapchain,
			UINT64_MAX,
			GetUsageSemaphore(),
			VK_NULL_HANDLE,
			&API->SwapchainCtx.ImageIndex
		);

		if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_ERROR_SURFACE_LOST_KHR) {
			API->Reset();
			return false;
		}

		if (res != VK_SUBOPTIMAL_KHR)
		{
			VkRenderAPI::Verify(res);
		}
		
		// Usage command buffer
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		const uint32_t FrameIndex = API->SwapchainCtx.FrameIndex;
		const uint32_t ImageIndex = API->SwapchainCtx.ImageIndex;

		res = vk(BeginCommandBuffer, GetCommandBuffer(FrameIndex), &beginInfo);
		VkRenderAPI::Verify(res);

		API->SwapchainCtx.Textures[ImageIndex].SetLayout(
			GetCommandBuffer(FrameIndex),
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		);

		res = vk(EndCommandBuffer, GetCommandBuffer(FrameIndex));
		VkRenderAPI::Verify(res);

		// Create submit info and submit
		VkSubmitInfo SubmitInfo{};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { GetUsageSemaphore() };
		SubmitInfo.waitSemaphoreCount = 1;
		SubmitInfo.pWaitSemaphores = waitSemaphores;

		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		SubmitInfo.pWaitDstStageMask = waitStages;

		SubmitInfo.commandBufferCount = 1;
		SubmitInfo.pCommandBuffers = &GetCommandBuffer(FrameIndex);

		res = vk(QueueSubmit, API->GetGraphicsQueue(), 1, &SubmitInfo, GetUsageFence());
		VkRenderAPI::Verify(res);

		// 2D update surface
		API->GetRenderer2D()->Swap();

		return true;
	}

	void VkScreenCommandEngine::Present()
	{
		API->GetRenderer2D()->Present();

		// Add MAX_FRAMES_IN_FLIGHT to receive the render buffer instead of usage buffer
		const uint32_t FrameIndex = API->SwapchainCtx.FrameIndex + MAX_FRAMES_IN_FLIGHT;
		const uint32_t ImageIndex = API->SwapchainCtx.ImageIndex;

		// Render command buffer
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		VkResult res = vk(BeginCommandBuffer, GetCommandBuffer(FrameIndex), &beginInfo);
		VkRenderAPI::Verify(res);

		// Set image layout through screen's usage command buffer
		API->SwapchainCtx.Textures[ImageIndex].SetLayout(
			GetCommandBuffer(FrameIndex),
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		);

		// Started in screen engine itself
		res = vk(EndCommandBuffer, GetCommandBuffer(FrameIndex));
		VkRenderAPI::Verify(res);

		// Create submit info and submit
		VkSubmitInfo SubmitInfo{};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore signalSemaphores[] = { GetRenderSemaphore() };
		SubmitInfo.signalSemaphoreCount = 1;
		SubmitInfo.pSignalSemaphores = signalSemaphores;

		SubmitInfo.commandBufferCount = 1;
		SubmitInfo.pCommandBuffers = &GetCommandBuffer(FrameIndex);

		res = vk(QueueSubmit, API->GetGraphicsQueue(), 1, &SubmitInfo, GetRenderFence());
		VkRenderAPI::Verify(res);

		// Create present info
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		VkSemaphore waitSemaphores[] = { GetRenderSemaphore() };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = waitSemaphores;

		VkSwapchainKHR swapChains[] = { API->SwapchainCtx.Swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &API->SwapchainCtx.ImageIndex;

		res = vk(QueuePresentKHR, API->GetPresentQueue(), &presentInfo);

		if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || API->IsResized())
		{
			API->Reset();
		}
		else
		{
			VkRenderAPI::Verify(res);
		}
	}
}