#pragma once

#include "Rendering/CommandEngine.h"
#include <vulkan/vulkan.h>

#include "Engine/Logger/Log.h"

struct RenderContextData;

namespace Engine
{
	class Engine_API VkCommandEngine : public CommandEngine
	{
	public:
		VkCommandEngine(Type type, const std::string& sName = "Command Engine");
		virtual ~VkCommandEngine();

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
		const VkCommandBuffer& GetCommandBuffer() const;

		const VkSemaphore& GetImageAcquiredSemaphore() const;
		const VkSemaphore& GetRenderCompleteSemaphore() const;

		const VkFence& GetFence() const;

	protected:
		// virtual void Signal() override;

		// Prepare to render next frame
		virtual void AdvanceFrame() override;

		VkCommandPool m_CommandPool;

		// For each swap chain frame buffer
		std::vector<VkCommandBuffer> m_CommandBuffers;

		std::vector<VkSemaphore> m_ImageAcquiredSemaphores;
		std::vector<VkSemaphore> m_RenderCompleteSemaphores;

		std::vector<VkFence> m_Fences;
	};
}
