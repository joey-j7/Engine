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
		VkCommandEngine(Type type, const std::string& sName = "Command Engine");
		virtual ~VkCommandEngine();

		void Init();
		void Deinit();
		
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
		void WaitForImageAcquirement();

		const VkCommandPool& GetCommandPool() const;

		const VkSemaphore& GetImageAcquiredSemaphore() const;
		const VkSemaphore& GetRenderCompleteSemaphore() const;

		const VkFence& GetFence() const;
		const VkFence& GetSemaphoreFence() const;

		// Get draw passes
		const std::vector<VkDrawPass*>& GetVkPasses() const { return m_VkPasses; };

		void ResetSemaphoreFence();

	protected:
		void CreateCommandPool();
		void CreateSyncObjects();
		
		// virtual void Signal() override;

		VkRenderAPI* API = nullptr;
		VkCommandPool m_CommandPool;

		std::vector<VkSemaphore> m_ImageAcquiredSemaphores;
		std::vector<VkSemaphore> m_RenderCompleteSemaphores;

		std::vector<VkFence> m_Fences;
		std::vector<VkFence> m_SemaphoreFences;

		std::vector<VkDrawPass*> m_VkPasses;

		bool m_Initialized = false;
	};
}
