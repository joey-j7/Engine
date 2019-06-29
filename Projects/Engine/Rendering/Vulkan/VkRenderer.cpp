#include "pch.h"

#if CB_RENDERING_API == CB_RENDERER_VULKAN

#include "VkRenderer.h"
#include "Engine/Application.h"


#ifdef CB_DEBUG
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
	(void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
	fprintf(stderr, "[vulkan] ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
	return VK_FALSE;
}
#endif // IMGUI_VULKAN_DEBUG_REPORT

namespace Engine {
	const char* Renderer::m_Name = "Vulkan";

	Renderer* Renderer::Create(const std::shared_ptr<RenderContextData>& contextData)
	{
		return new VkRenderer(contextData);
	}

	VkRenderer::VkRenderer(const std::shared_ptr<RenderContextData>& contextData) : Renderer(contextData)
	{
		Init();
	}

	VkRenderer::~VkRenderer()
	{
		int err = vkDeviceWaitIdle(m_pContextData->Device);
		Verify(err);
	}

	void VkRenderer::Clear()
	{

	}

	void VkRenderer::Init()
	{
		VkResult err;

		// Setup Vulkan
		CB_CORE_ASSERT(glfwVulkanSupported(), "GLFW: Vulkan Not Supported\n");

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
			const char* layers[] = { "VK_LAYER_LUNARG_standard_validation" };
			create_info.enabledLayerCount = 1;
			create_info.ppEnabledLayerNames = layers;

			// Enable debug report extension (we need additional storage, so we duplicate the user array to add our new extension to it)
			const char** extensions_ext = (const char**)malloc(sizeof(const char*) * (extensions_count + 1));
			memcpy(extensions_ext, extensions, extensions_count * sizeof(const char*));
			extensions_ext[extensions_count] = "VK_EXT_debug_report";
			create_info.enabledExtensionCount = extensions_count + 1;
			create_info.ppEnabledExtensionNames = extensions_ext;

			// Create Vulkan Instance
			err = vkCreateInstance(&create_info, m_pContextData->Allocator, &m_pContextData->Instance);
			Verify(err);
			free(extensions_ext);

			// Get the function pointer (required for any extensions)
			auto vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_pContextData->Instance, "vkCreateDebugReportCallbackEXT");
			CB_CORE_ASSERT(vkCreateDebugReportCallbackEXT != NULL, "Function pointer extension is not supported!");

			// Setup the debug report callback
			VkDebugReportCallbackCreateInfoEXT debug_report_ci = {};
			debug_report_ci.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
			debug_report_ci.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
			debug_report_ci.pfnCallback = debug_report;
			debug_report_ci.pUserData = NULL;
			err = vkCreateDebugReportCallbackEXT(m_pContextData->Instance, &debug_report_ci, m_pContextData->Allocator, &m_pContextData->DebugReport);
			Verify(err);
#else
			// Create Vulkan Instance without any debug feature
			err = vkCreateInstance(&create_info, m_pContextData->Allocator, &m_pContextData->Instance);
			Verify(err);
#endif
		}

		// Select GPU
		{
			uint32_t gpu_count;
			err = vkEnumeratePhysicalDevices(m_pContextData->Instance, &gpu_count, NULL);
			Verify(err);

			VkPhysicalDevice* gpus = (VkPhysicalDevice*)malloc(sizeof(VkPhysicalDevice) * gpu_count);
			err = vkEnumeratePhysicalDevices(m_pContextData->Instance, &gpu_count, gpus);
			Verify(err);

			// If a number >1 of GPUs got reported, you should find the best fit GPU for your purpose
			// e.g. VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU if available, or with the greatest memory available, etc.
			// for sake of simplicity we'll just take the first one, assuming it has a graphics queue family.
			m_pContextData->PhysicalDevice = gpus[0];
			free(gpus);
		}

		// Select graphics queue family
		{
			uint32_t count;
			vkGetPhysicalDeviceQueueFamilyProperties(m_pContextData->PhysicalDevice, &count, NULL);
			VkQueueFamilyProperties* queues = (VkQueueFamilyProperties*)malloc(sizeof(VkQueueFamilyProperties) * count);
			vkGetPhysicalDeviceQueueFamilyProperties(m_pContextData->PhysicalDevice, &count, queues);
			for (uint32_t i = 0; i < count; i++)
				if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					m_pContextData->QueueFamily = i;
					break;
				}
			free(queues);
			CB_CORE_ASSERT(m_pContextData->QueueFamily != -1, "Invalid graphic queue family!");
		}

		// Create Logical Device (with 1 queue)
		{
			int device_extension_count = 1;
			const char* device_extensions[] = { "VK_KHR_swapchain" };
			const float queue_priority[] = { 1.0f };
			VkDeviceQueueCreateInfo queue_info[1] = {};
			queue_info[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_info[0].queueFamilyIndex = m_pContextData->QueueFamily;
			queue_info[0].queueCount = 1;
			queue_info[0].pQueuePriorities = queue_priority;
			VkDeviceCreateInfo create_info = {};
			create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			create_info.queueCreateInfoCount = sizeof(queue_info) / sizeof(queue_info[0]);
			create_info.pQueueCreateInfos = queue_info;
			create_info.enabledExtensionCount = device_extension_count;
			create_info.ppEnabledExtensionNames = device_extensions;
			err = vkCreateDevice(m_pContextData->PhysicalDevice, &create_info, m_pContextData->Allocator, &m_pContextData->Device);
			Verify(err);
			vkGetDeviceQueue(m_pContextData->Device, m_pContextData->QueueFamily, 0, &m_pContextData->Queue);
		}

		// Create Descriptor Pool
		{
			VkDescriptorPoolSize pool_sizes[] =
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
			};

			VkDescriptorPoolCreateInfo pool_info = {};
			pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			pool_info.maxSets = 1000 * ((int)(sizeof(pool_sizes) / sizeof(*pool_sizes)));
			pool_info.poolSizeCount = (uint32_t)((int)(sizeof(pool_sizes) / sizeof(*pool_sizes)));
			pool_info.pPoolSizes = pool_sizes;
			err = vkCreateDescriptorPool(m_pContextData->Device, &pool_info, m_pContextData->Allocator, &m_pContextData->DescriptorPool);
			Verify(err);
		}

		auto& window = Application::Get().GetRenderContext().GetWindow();

		// Create Window Surface
		auto vkAllocator = m_pContextData->Allocator;
		err = glfwCreateWindowSurface(m_pContextData->Instance, (GLFWwindow*)window.GetWindow(), vkAllocator, &m_pContextData->Surface);
		Verify(err);
	}
}

#endif