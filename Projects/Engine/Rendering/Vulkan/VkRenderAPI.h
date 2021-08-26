#pragma once

#include "Rendering/RenderAPI.h"
#include <vulkan/vulkan.h>

#include "Objects/VkShaderProgram.h"

#include "VkSwapchainContext.h"

namespace Engine {
	class VkCommandEngine;
	class VkScreenCommandEngine;

#define vk(_vkName, ...) ((PFN_vk##_vkName)glfwGetInstanceProcAddress(VkRenderAPI::Get().Instance, "vk" #_vkName))(__VA_ARGS__);
#define MAX_FRAMES_IN_FLIGHT 2
	
	class Engine_API VkRenderAPI : public RenderAPI
	{
	public:
		struct QueueFamilyIndices {
			std::optional<uint32_t> GraphicsFamily;
			std::optional<uint32_t> PresentFamily;

			bool isComplete() {
				return GraphicsFamily.has_value() && PresentFamily.has_value();
			}
		};
		
		VkRenderAPI(RenderContext& renderContext);
		~VkRenderAPI() override;
		static VkRenderAPI& Get() { return *s_Instance; }

		virtual ShaderProgram* Create(const ShaderProgram::Descriptor& descriptor) override { return new VkShaderProgram(descriptor); }

		static void Verify(VkResult err)
		{
			if (err == 0) return;

			if (err < 0) { CB_CORE_ASSERT(false, "VkResult " + std::to_string(err)); }
			else { CB_CORE_ERROR(("VkResult " + std::to_string(err)).c_str()); }
		}
		
		virtual bool Init() override;
		virtual bool Deinit() override;

		virtual bool Swap() override;
		virtual void Present() override;
;
		virtual CommandEngine* GetCommandEngine(const String& sName) override;

		VkQueue GetGraphicsQueue() const { return GraphicsQueue; }
		VkQueue GetPresentQueue() const { return PresentQueue; }

		VkPipelineStageFlags GetPipelineStageFlags(VkImageLayout layout);
		
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

		VkInstance Instance = VK_NULL_HANDLE;
		VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
		VkDevice Device = VK_NULL_HANDLE;
		uint32_t QueueFamily = UINT_MAX;
		VkDebugReportCallbackEXT DebugReport = VK_NULL_HANDLE;
		VkSurfaceKHR Surface = VK_NULL_HANDLE;
		VkSwapchainContext SwapchainCtx;
		VkPhysicalDeviceFeatures Features;
		
	protected:
		void CreateInstance();
		std::vector<const char*> GetRequiredExtensions();

		const std::vector<const char*> DeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

#if defined(CB_DEBUG) && defined(CB_PLATFORM_WINDOWS)
		static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		bool CheckValidationLayerSupport();
		void SetupDebugMessages();
		static VkResult CreateDebugUtilsMessengerEXT(
			VkInstance instance,
		    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		    const VkAllocationCallbacks* pAllocator,
		    VkDebugUtilsMessengerEXT* pDebugMessenger
		);

		VkDebugUtilsMessengerEXT DebugMessenger;
		
		const std::vector<const char*> ValidationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
#endif

		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();

		bool IsDeviceSuitable(VkPhysicalDevice device);
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device) const;

		VkQueue GraphicsQueue;
		VkQueue PresentQueue;
		
		std::unordered_map<String, VkCommandEngine*> m_CommandEngines;
		VkScreenCommandEngine* m_ScreenCommandEngine = nullptr;

		bool m_InitializedOnce = false;
		
		static VkRenderAPI* s_Instance;
	};
}