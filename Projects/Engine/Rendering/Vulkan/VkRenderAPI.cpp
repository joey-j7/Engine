#include "pch.h"

#include "VkRenderAPI.h"

#include "VkCommandEngine.h"
#include "Files/VkShaderFile.h"

#include "Rendering/RenderContext.h"

#ifdef CB_DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
	(void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
	fprintf(stderr, "[vulkan] ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
	return VK_FALSE;
}
#endif // IMGUI_VULKAN_DEBUG_REPORT

namespace Engine
{
	VkRenderAPI* VkRenderAPI::s_Instance = nullptr;

	VkRenderAPI::VkRenderAPI(RenderContext& renderContext) : RenderAPI(renderContext)
	{
		CB_CORE_ASSERT(!s_Instance, "VkRenderAPI already exists!");
		s_Instance = this;

		// m_pDatabase->Add<VkModelFile>({ "fbx", "obj", "vox" });
		// m_pDatabase->Add<VkTextureFile>({ "png", "jpg", "bmp" });
		m_pDatabase->Add<VkShaderFile>({ "vspv" });

		Init();
	}

	VkRenderAPI::~VkRenderAPI()
	{
		VkResult err = vkDeviceWaitIdle(Device);
		Verify(err);

		for (auto& it : m_CommandEngines)
		{
			delete it.second;
		}

		m_pDatabase.reset();
		SwapchainCtx.Deinit();

		vkDestroySurfaceKHR(Instance, Surface, nullptr);

		vkDestroyDevice(Device, Allocator);
		vkDestroyInstance(Instance, Allocator);
	}

	void VkRenderAPI::Swap()
	{
		VkResult err = vkAcquireNextImageKHR(
			Device,
			SwapchainCtx.Swapchain,
			std::numeric_limits<uint64_t>::max(),
			m_ScreenCommandEngine->GetImageAcquiredSemaphore(),
			VK_NULL_HANDLE,
			&SwapchainCtx.FrameIndex
		);

		Verify(err);
	}

	void VkRenderAPI::Present()
	{
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		VkSemaphore signalSemaphores[] = { m_ScreenCommandEngine->GetRenderCompleteSemaphore() };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { SwapchainCtx.Swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &SwapchainCtx.FrameIndex;

		vkQueuePresentKHR(SwapchainCtx.PresentQueue, &presentInfo);

		SwapchainCtx.SemaphoreIndex = (SwapchainCtx.SemaphoreIndex + 1) % SwapchainCtx.FrameCount;
	}

	CommandEngine* VkRenderAPI::GetCommandEngine(const std::string& sName)
	{
		auto it = m_CommandEngines.find(sName);

		if (it == m_CommandEngines.end())
			return nullptr;

		return it->second;
	}

	bool VkRenderAPI::Init()
	{
		if (m_bInitialized)
			return false;

		m_sName = "Vulkan";

		VkResult err;

		// Setup Vulkan
		CB_CORE_ASSERT(glfwVulkanSupported(), "GLFW: Vulkan is not supported by the system");

		uint32_t extensions_count = 0;
		const char** extensions = glfwGetRequiredInstanceExtensions(&extensions_count);

		// Create Vulkan Instance
		{
			VkInstanceCreateInfo create_info = {};
			create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			create_info.enabledExtensionCount = extensions_count;
			create_info.ppEnabledExtensionNames = extensions;

#if defined(CB_DEBUG) && !defined(CB_PLATFORM_ANDROID)
			// Enabling multiple validation layers grouped as LunarG standard validation
			const char* layers[] = { "VK_LAYER_KHRONOS_validation" };
			create_info.enabledLayerCount = 1;
			create_info.ppEnabledLayerNames = layers;

			// Enable debug report extension (we need additional storage, so we duplicate the user array to add our new extension to it)
			const char** extensions_ext = (const char**)malloc(sizeof(const char*) * (extensions_count + 1));
			memcpy(extensions_ext, extensions, extensions_count * sizeof(const char*));
			extensions_ext[extensions_count] = "VK_EXT_debug_report";
			create_info.enabledExtensionCount = extensions_count + 1;
			create_info.ppEnabledExtensionNames = extensions_ext;

			// Create Vulkan Instance
			err = vkCreateInstance(&create_info, Allocator, &Instance);
			Verify(err);
			free(extensions_ext);

			// Get the function pointer (required for any extensions)
			auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(Instance, "vkCreateDebugReportCallbackEXT");
			CB_CORE_ASSERT(vkCreateDebugReportCallbackEXT != NULL, "Function pointer extension is not supported!");

			// Setup the debug report callback
			VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
			debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
			debug_report_ci.pfnCallback = debug_report;
			debug_report_ci.pUserData = NULL;
			err = vkCreateDebugReportCallbackEXT(Instance, &debug_report_ci, Allocator, &DebugReport);
			Verify(err);
#else
			// Create Vulkan Instance without any debug feature
			err = vkCreateInstance(&create_info, Allocator, &Instance);
			Verify(err);
#endif
		}

		// Select GPU
		{
			uint32_t gpu_count;
			err = vkEnumeratePhysicalDevices(Instance, &gpu_count, NULL);
			Verify(err);

			VkPhysicalDevice* gpus = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpu_count);
			err = vkEnumeratePhysicalDevices(Instance, &gpu_count, gpus);
			Verify(err);

			// If a number >1 of GPUs got reported, you should find the best fit GPU for your purpose
			// e.g. VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU if available, or with the greatest memory available, etc.
			// for sake of simplicity we'll just take the first one, assuming it has a graphics queue family.
			PhysicalDevice = gpus[0];
			free(gpus);
		}

		// Select graphics queue family
		{
			uint32_t count;
			vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &count, NULL);
			VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
			vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &count, queues);
			for (uint32_t i = 0; i < count; i++)
				if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					QueueFamily = i;
					break;
				}
			free(queues);
			CB_CORE_ASSERT(QueueFamily != -1, "Invalid graphic queue family!");
		}

		// Create Logical Device (with 1 queue)
		{
			int device_extension_count = 1;
			const char* device_extensions[] = { "VK_KHR_swapchain" };
			const float queue_priority[] = { 1.0f };
			VkDeviceQueueCreateInfo queue_info[1] = {};
			queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_info[0].queueFamilyIndex = QueueFamily;
			queue_info[0].queueCount = 1;
			queue_info[0].pQueuePriorities = queue_priority;
			VkDeviceCreateInfo create_info = {};
			create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
			create_info.pQueueCreateInfos = queue_info;
			create_info.enabledExtensionCount = device_extension_count;
			create_info.ppEnabledExtensionNames = device_extensions;
			err = vkCreateDevice(PhysicalDevice, &create_info, Allocator, &Device);
			Verify(err);
			vkGetDeviceQueue(Device, QueueFamily, 0, &Queue);
		}

		/* Create Window Surface */
		auto& window = m_pRenderContext->GetWindow();

		err = glfwCreateWindowSurface(
			Instance,
			(GLFWwindow*)window.GetWindow(),
			Allocator,
			&Surface
		);

		Verify(err);

		/* Create swap chain */
		SwapchainCtx.Init(*this);

		m_CommandEngines.emplace("Screen", new VkCommandEngine(CommandEngine::E_DIRECT, "Screen"));
		m_ScreenCommandEngine = m_CommandEngines["Screen"];

		return RenderAPI::Init();
	}
}