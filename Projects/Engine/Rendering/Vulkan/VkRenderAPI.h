#pragma once

#include "Rendering/RenderAPI.h"
#include <vulkan/vulkan.h>

#include "Objects/VkShaderProgram.h"

#include "VkSwapchainContext.h"

namespace Engine {
	class VkCommandEngine;

#define vk(_vkName, ...) ((PFN_vk##_vkName)glfwGetInstanceProcAddress(VkRenderAPI::Get().Instance, "vk" #_vkName))(__VA_ARGS__);

	class Engine_API VkRenderAPI : public RenderAPI
	{
	public:
		VkRenderAPI(RenderContext& renderContext);
		~VkRenderAPI() override;
		static VkRenderAPI& Get() { return *s_Instance; }

		virtual ShaderProgram* Create(const ShaderProgram::Descriptor& descriptor) override { return new VkShaderProgram(descriptor); }

		static void Verify(VkResult err)
		{
			if (err == 0) return;
			else if (err < 0) { CB_CORE_ASSERT(false, "VkResult " + std::to_string(err)); }
			else { CB_CORE_ERROR(("VkResult " + std::to_string(err)).c_str()); }
		}

		virtual void Swap() override;
		virtual void Present() override;

		virtual void Suspend() override;
		virtual void Resume() override;

		virtual CommandEngine* GetCommandEngine(const std::string& sName) override;

		VkAllocationCallbacks* Allocator = nullptr;
		VkInstance Instance = VK_NULL_HANDLE;
		VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
		VkDevice Device = VK_NULL_HANDLE;
		uint32_t QueueFamily = UINT_MAX;
		VkQueue Queue = VK_NULL_HANDLE;
		VkDebugReportCallbackEXT DebugReport = VK_NULL_HANDLE;
		VkSurfaceKHR Surface = VK_NULL_HANDLE;
		VkSwapchainContext SwapchainCtx;
		VkPhysicalDeviceFeatures Features;

	protected:
		virtual bool Init() override;

		std::unordered_map<std::string, VkCommandEngine*> m_CommandEngines;
		VkCommandEngine* m_ScreenCommandEngine = nullptr;

		static VkRenderAPI* s_Instance;
	};
}