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

	VkCommandEngine::VkCommandEngine(Type type, const String& sName) : CommandEngine(type, sName)
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

		for (auto Pass : GetVkPasses())
		{
			Pass->Deinit();
		}

		// Clear all command buffers
		vk(
			FreeCommandBuffers,
			api.Device,
			GetCommandPool(),
			static_cast<uint32_t>(
				m_CommandBuffers.size()
			),
			m_CommandBuffers.data()
		);

		m_CommandBuffers.clear();

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			vk(DestroySemaphore, api.Device, m_RenderSemaphores[i], nullptr);
			vk(DestroySemaphore, api.Device, m_UsageSemaphores[i], nullptr);
			
			vk(DestroyFence, api.Device, m_UsageFences[i], nullptr);
		}

		vk(DestroyCommandPool, api.Device, m_CommandPool, nullptr);

		m_UsageSemaphores.clear();
		m_RenderSemaphores.clear();
		m_UsageFences.clear();
		m_RenderFences.clear();

		m_State = E_IDLE;
		m_uiFenceValue = 0;

		m_Initialized = false;
	}

	void VkCommandEngine::CreateCommandPool() {
		VkRenderAPI::QueueFamilyIndices queueFamilyIndices = API->FindQueueFamilies(API->PhysicalDevice);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.GraphicsFamily.value();
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		const auto& res = vk(CreateCommandPool, API->Device, &poolInfo, nullptr, &m_CommandPool);
		VkRenderAPI::Verify(res);
	}

	void VkCommandEngine::CreateSyncObjects() {
		const uint32_t frameCount = MAX_FRAMES_IN_FLIGHT;
		
		m_UsageSemaphores.resize(frameCount);
		m_RenderSemaphores.resize(frameCount);
		m_UsageFences.resize(frameCount);
		m_RenderFences.resize(frameCount);
		
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < frameCount; i++) {
			auto res = vk(CreateSemaphore, API->Device, &semaphoreInfo, nullptr, &m_UsageSemaphores[i]);
			VkRenderAPI::Verify(res);
			res = vk(CreateSemaphore, API->Device, &semaphoreInfo, nullptr, &m_RenderSemaphores[i]);
			VkRenderAPI::Verify(res);
			res = vk(CreateFence, API->Device, &fenceInfo, nullptr, &m_UsageFences[i]);
			VkRenderAPI::Verify(res);
			res = vk(CreateFence, API->Device, &fenceInfo, nullptr, &m_RenderFences[i]);
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

		m_State = E_IDLE;
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

		if (m_VkPasses.size() == 0)
			return;

		/* Submit graphics queue */
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { GetUsageSemaphore() };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = static_cast<uint32_t>(m_Passes.size());

		const VkCommandBuffer** buffers = new const VkCommandBuffer * [m_Passes.size() + m_CommandBuffers.size()];

		for (uint32_t i = 0; i < m_VkPasses.size(); ++i)
		{
			buffers[i] = &m_VkPasses[i]->GetCommandBuffer();
		}

		submitInfo.pCommandBuffers = *buffers;

		VkSemaphore signalSemaphores[] = { GetRenderSemaphore() };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		const VkResult res = vk(QueueSubmit, VkRenderAPI::Get().GetGraphicsQueue(), 1, &submitInfo, GetUsageFence());
		VkRenderAPI::Verify(res);

		delete[] buffers;
	}

	void VkCommandEngine::WaitForGPU()
	{
		VkFence Fences[2] = { GetUsageFence(), GetRenderFence() };
		VkResult res = vk(WaitForFences, API->Device, 2, Fences, VK_TRUE, UINT64_MAX);
		VkRenderAPI::Verify(res);

		res = vk(ResetFences, API->Device, 2, Fences);
		VkRenderAPI::Verify(res);
	}

	const VkCommandPool& VkCommandEngine::GetCommandPool() const
	{
		return m_CommandPool;
	}

	const VkCommandBuffer& VkCommandEngine::GetCommandBuffer(uint32_t Index) const
	{
		return m_CommandBuffers[Index];
	}

	void VkCommandEngine::CreateCommandBuffer(VkStructureType Type, VkCommandBufferLevel Level)
	{
		m_CommandBuffers.push_back({});
		
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = Type;
		allocInfo.commandPool = GetCommandPool();
		allocInfo.level = Level;
		allocInfo.commandBufferCount = 1;

		const VkResult res = vk(AllocateCommandBuffers, API->Device, &allocInfo, &m_CommandBuffers.back());
		VkRenderAPI::Verify(res);
	}

	const VkSemaphore& VkCommandEngine::GetUsageSemaphore() const
	{
		return m_UsageSemaphores[VkRenderAPI::Get().SwapchainCtx.FrameIndex];
	}

	const VkSemaphore& VkCommandEngine::GetRenderSemaphore() const
	{
		return m_RenderSemaphores[VkRenderAPI::Get().SwapchainCtx.FrameIndex];
	}

	const VkFence& VkCommandEngine::GetUsageFence() const
	{
		return m_UsageFences[VkRenderAPI::Get().SwapchainCtx.FrameIndex];
	}

	const VkFence& VkCommandEngine::GetRenderFence() const
	{
		return m_RenderFences[VkRenderAPI::Get().SwapchainCtx.FrameIndex];
	}
}