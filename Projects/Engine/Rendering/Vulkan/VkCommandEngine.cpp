#include "pch.h"

#include "VkCommandEngine.h"

#include "VkRenderAPI.h"

#include "Passes/VkDrawPass.h"

namespace Engine
{
	DrawPass* VkCommandEngine::Create(const ShaderProgram::Descriptor& shaderPipeline)
	{
		VkDrawPass* pass = new VkDrawPass(*this, shaderPipeline);
		m_Passes.push_back(pass);
		m_VkPasses.push_back(pass);

		return pass;
	}

	VkCommandEngine::VkCommandEngine(Type type, const std::string& sName) : CommandEngine(type, sName)
	{
		API = &VkRenderAPI::Get();

		Init();
	}

	void VkCommandEngine::Init()
	{
		if (m_Initialized)
			return;

		CreateCommandPool();
		CreateSyncObjects();

		m_Initialized = true;
	}

	void VkCommandEngine::Deinit()
	{
		if (!m_Initialized)
			return;

		auto& api = VkRenderAPI::Get();

		for (uint32_t i = 0; i < api.SwapchainCtx.FrameCount; ++i)
		{
			vk(DestroySemaphore, api.Device, m_RenderCompleteSemaphores[i], nullptr);
			vk(DestroySemaphore, api.Device, m_ImageAcquiredSemaphores[i], nullptr);
			
			vk(DestroyFence, api.Device, m_Fences[i], nullptr);
		}

		vk(DestroyCommandPool, api.Device, m_CommandPool, nullptr);

		m_ImageAcquiredSemaphores.clear();
		m_RenderCompleteSemaphores.clear();
		m_Fences.clear();
		m_SemaphoreFences.clear();

		m_State = E_IDLE;
		m_uiFenceValue = 0;

		m_Initialized = false;
	}

	void VkCommandEngine::ResetSemaphoreFence()
	{
		m_SemaphoreFences.resize(API->SwapchainCtx.Views.size(), VK_NULL_HANDLE);
	}

	void VkCommandEngine::CreateCommandPool() {
		VkRenderAPI::QueueFamilyIndices queueFamilyIndices = API->FindQueueFamilies(API->PhysicalDevice);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();

		const auto& res = vk(CreateCommandPool, API->Device, &poolInfo, nullptr, &m_CommandPool);
		VkRenderAPI::Verify(res);
	}

	void VkCommandEngine::CreateSyncObjects() {
		const uint32_t frameCount = API->SwapchainCtx.FrameCount;
		
		m_ImageAcquiredSemaphores.resize(frameCount);
		m_RenderCompleteSemaphores.resize(frameCount);
		m_Fences.resize(frameCount);
		m_SemaphoreFences.resize(API->SwapchainCtx.Views.size(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < frameCount; i++) {
			auto res = vk(CreateSemaphore, API->Device, &semaphoreInfo, nullptr, &m_ImageAcquiredSemaphores[i]);
			VkRenderAPI::Verify(res);
			res = vk(CreateSemaphore, API->Device, &semaphoreInfo, nullptr, &m_RenderCompleteSemaphores[i]);
			VkRenderAPI::Verify(res);
			res = vk(CreateFence, API->Device, &fenceInfo, nullptr, &m_Fences[i]);
			VkRenderAPI::Verify(res);
		}
	}

	VkCommandEngine::~VkCommandEngine()
	{
		Deinit();
	}

	void VkCommandEngine::Reset()
	{
		if (m_State == E_RECORDING || m_State == E_IDLE)
			return;
	}

	void VkCommandEngine::Begin()
	{
		if (m_State != E_IDLE) return;
		m_State = E_RECORDING;
	}

	void VkCommandEngine::Execute()
	{
		if (m_State != E_RECORDING) return;
		m_State = E_EXECUTING;

		WaitForImageAcquirement();
		
		/* Submit graphics queue */
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		const uint32_t frame = API->SwapchainCtx.FrameIndex;
		
		VkSemaphore waitSemaphores[] = { m_ImageAcquiredSemaphores[frame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = m_Passes.size();

		const VkCommandBuffer** buffers = new const VkCommandBuffer * [m_Passes.size()];

		for (uint32_t i = 0; i < m_VkPasses.size(); ++i)
		{
			buffers[i] = &m_VkPasses[i]->GetCommandBuffer();
		}

		submitInfo.pCommandBuffers = *buffers;

		VkSemaphore signalSemaphores[] = { GetRenderCompleteSemaphore() };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		auto& api = VkRenderAPI::Get();

		// Command list allocators can only be reset when the associated 
		// command lists have finished execution on the GPU; apps should use 
		// fences to determine GPU execution progress.
		VkResult err = vk(ResetFences, api.Device, 1, &GetFence());
		VkRenderAPI::Verify(err);

		err = vk(QueueSubmit, VkRenderAPI::Get().GetGraphicsQueue(), 1, &submitInfo, GetFence());
		VkRenderAPI::Verify(err);

		delete[] buffers;
	}

	void VkCommandEngine::WaitForGPU()
	{
		vk(WaitForFences, API->Device, 1, &GetFence(), VK_TRUE, UINT64_MAX);
	}

	void VkCommandEngine::WaitForImageAcquirement()
	{
		const uint32_t semaphore = API->SwapchainCtx.SemaphoreIndex;

		if (m_SemaphoreFences[semaphore] != VK_NULL_HANDLE) {
			vk(WaitForFences, API->Device, 1, &m_SemaphoreFences[semaphore], VK_TRUE, UINT64_MAX);
		}

		const uint32_t frame = API->SwapchainCtx.FrameIndex;
		m_SemaphoreFences[semaphore] = m_Fences[frame];
	}

	const VkCommandPool& VkCommandEngine::GetCommandPool() const
	{
		return m_CommandPool;
	}

	const VkSemaphore& VkCommandEngine::GetImageAcquiredSemaphore() const
	{
		return m_ImageAcquiredSemaphores[VkRenderAPI::Get().SwapchainCtx.FrameIndex];
	}

	const VkSemaphore& VkCommandEngine::GetRenderCompleteSemaphore() const
	{
		return m_RenderCompleteSemaphores[VkRenderAPI::Get().SwapchainCtx.FrameIndex];
	}

	const VkFence& VkCommandEngine::GetFence() const
	{
		return m_Fences[VkRenderAPI::Get().SwapchainCtx.FrameIndex];
	}

	const VkFence& VkCommandEngine::GetSemaphoreFence() const
	{
		return m_SemaphoreFences[VkRenderAPI::Get().SwapchainCtx.SemaphoreIndex];
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
	//		VkResult err = vk(CreateFence, m_pContextData->Device, &fenceInfo, nullptr, &m_Fences.back());
	//		VkRenderAPI::Verify(err);
	//	}
	//}
}