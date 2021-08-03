#pragma once

#include "Rendering/CommandEngine.h"
#include <vulkan/vulkan.h>

#include "Engine/Logger/Log.h"
#include "Passes/VkDrawPass.h"

struct RenderContextData;

namespace Engine
{
	class VkRenderAPI;
	
	class Engine_API VkCommandEngine : public CommandEngine
	{
	public:
		VkCommandEngine(Type type, const String& sName = "Command Engine");
		virtual ~VkCommandEngine();

		virtual void Init();
		virtual void Deinit();
		
		virtual DrawPass* Create(const ShaderProgram::Descriptor& shaderPipeline) override;

		// Reset allocator
		virtual void Reset() override;

		// Reset command List
		virtual void Begin() override;

		// Close command list and execute
		virtual void Execute() override;

		// Apply all pending barriers
		virtual void ApplyBarriers() override
		{
			CB_CORE_ASSERT(false, "Not implemented");
		};

		// Wait until GPU has finished executing
		virtual void WaitForGPU() override;

		const VkCommandPool& GetCommandPool() const;
		
		const VkCommandBuffer& GetCommandBuffer(uint32_t Index) const;
		void CreateCommandBuffer(
			VkStructureType Type = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			VkCommandBufferLevel Level = VK_COMMAND_BUFFER_LEVEL_PRIMARY
		);

		const VkSemaphore& GetUsageSemaphore() const;
		const VkSemaphore& GetRenderSemaphore() const;

		const VkFence& GetUsageFence() const;
		const VkFence& GetRenderFence() const;

		// Get draw passes
		const std::vector<VkDrawPass*>& GetVkPasses() const { return m_VkPasses; };

	protected:
		void CreateCommandPool();
		void CreateSyncObjects();
		
		// virtual void Signal() override;

		VkRenderAPI* API = nullptr;

		VkCommandPool m_CommandPool;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		std::vector<VkSemaphore> m_UsageSemaphores;
		std::vector<VkSemaphore> m_RenderSemaphores;

		std::vector<VkFence> m_UsageFences;
		std::vector<VkFence> m_RenderFences;

		std::vector<VkDrawPass*> m_VkPasses;

		bool m_Initialized = false;
	};
}
