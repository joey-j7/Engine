#pragma once

#include "pch.h"

struct RenderContextData
{
#if CB_RENDERING_API == CB_RENDERER_OPENGL
#elif CB_RENDERING_API == CB_RENDERER_VULKAN
	VkAllocationCallbacks* Allocator = NULL;
	VkInstance Instance = VK_NULL_HANDLE;
	VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
	VkDevice Device = VK_NULL_HANDLE;
	uint32_t QueueFamily = (uint32_t)-1;
	VkQueue Queue = VK_NULL_HANDLE;
	VkDebugReportCallbackEXT DebugReport = VK_NULL_HANDLE;
	VkPipelineCache PipelineCache = VK_NULL_HANDLE;
	VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
	VkSurfaceKHR Surface = VK_NULL_HANDLE;
#endif
};