#pragma once

#if CB_RENDERING_API == CB_RENDERER_VULKAN

#include "Engine/Renderer.h"

#if CB_RENDERING_API == CB_RENDERER_VULKAN
#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#endif
#endif
#include "GLFW/glfw3.h"

namespace Engine {
	class VkRenderer : public Renderer
	{
	public:
		VkRenderer();
		virtual void Clear() override;

		void Verify(VkResult err)
		{
			if (err == 0) return;
			CB_CORE_ERROR("VkResult %d\n", err);
			if (err < 0) abort();
		}

	private:
		virtual void Init();

		VkAllocationCallbacks* m_Allocator = NULL;
		VkInstance m_Instance = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device = VK_NULL_HANDLE;
		uint32_t m_QueueFamily = (uint32_t)-1;
		VkQueue m_Queue = VK_NULL_HANDLE;
		VkDebugReportCallbackEXT m_DebugReport = VK_NULL_HANDLE;
		VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;
		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
	};
}

#endif