#include "pch.h"

#include "VkRenderAPI.h"

#include "VkCommandEngine.h"
#include "Files/VkShaderFile.h"

#include "Renderers/VkRenderer2D.h"
#include "Renderers/VkRenderer3D.h"

#include "Rendering/RenderContext.h"

#include <set>

#include "VkScreenCommandEngine.h"
#include "Passes/VkDrawPass.h"

#if CB_DEBUG && defined(CB_PLATFORM_WINDOWS)
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	const String s = String("[VULKAN] ") + pCallbackData->pMessage + '\0';
	
	switch (messageSeverity)
	{
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		CB_CORE_ERROR(s.c_str());
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		CB_CORE_WARN(s.c_str());
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		CB_CORE_INFO(s.c_str());
		break;
	case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
	default:
		CB_CORE_TRACE(s.c_str());
		break;
	}
	
	return VK_FALSE;
}
#endif 

namespace Engine
{
	VkRenderAPI* VkRenderAPI::s_Instance = nullptr;

	VkRenderAPI::VkRenderAPI(RenderContext& renderContext) : RenderAPI(renderContext), SwapchainCtx(*this)
	{
		CB_CORE_ASSERT(!s_Instance, "VkRenderAPI already exists!");
		
		// m_pDatabase->Add<VkModelFile>({ "fbx", "obj", "vox" });
		// m_pDatabase->Add<VkTextureFile>({ "png", "jpg", "bmp" });
		FileDatabase::Get().Add<VkShaderFile>({ "vspv" });

		Init();
	}

	VkRenderAPI::~VkRenderAPI()
	{
		Deinit();

		m_pRenderer2D.reset();
		m_pRenderer3D.reset();

		for (auto it1 : m_CommandEngines)
		{
			delete it1.second;
		}

		m_CommandEngines.clear();

		if (s_Instance == this)
			s_Instance = nullptr;
	}

	bool VkRenderAPI::Swap()
	{
		SwapchainCtx.FrameIndex = (SwapchainCtx.FrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
		return m_ScreenCommandEngine->Swap();
	}

	void VkRenderAPI::Present()
	{
		// Transition swapchain image barrier to PRESENT
		m_ScreenCommandEngine->Present();
	}
	
	CommandEngine* VkRenderAPI::GetCommandEngine(const String& sName)
	{
		auto it = m_CommandEngines.find(sName);

		if (it == m_CommandEngines.end())
			return nullptr;

		return it->second;
	}

	VkPipelineStageFlags VkRenderAPI::GetPipelineStageFlags(const VkImageLayout layout)
	{
		switch (layout) {
		case VK_IMAGE_LAYOUT_UNDEFINED:
			return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		case VK_IMAGE_LAYOUT_GENERAL:
			return VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		case VK_IMAGE_LAYOUT_PREINITIALIZED:
			return VK_PIPELINE_STAGE_HOST_BIT;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			return VK_PIPELINE_STAGE_TRANSFER_BIT;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			return VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: {
			VkPipelineStageFlags flags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
				VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;

			if (Features.tessellationShader) {
				flags |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
					VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
			}
			if (Features.geometryShader) {
				flags |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
			}
			return flags;
		}
		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
			return VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		}
		return 0;
	}

	VkRenderAPI::QueueFamilyIndices VkRenderAPI::FindQueueFamilies(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vk(GetPhysicalDeviceQueueFamilyProperties, device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vk(GetPhysicalDeviceQueueFamilyProperties, device, &queueFamilyCount, queueFamilies.data());

		int32_t i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.GraphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vk(GetPhysicalDeviceSurfaceSupportKHR, device, i, Surface, &presentSupport);

			if (presentSupport) {
				indices.PresentFamily = i;
			}

			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}
	
	bool VkRenderAPI::Init()
	{
		if (m_bInitialized)
			return false;

		s_Instance = this;

		if (!m_InitializedOnce)
		{
			m_sName = "Vulkan";

			// Setup Vulkan
			CB_CORE_ASSERT(glfwVulkanSupported(), "GLFW: Vulkan is not supported by the system");

			CreateInstance();

#if defined(CB_DEBUG) && defined(CB_PLATFORM_WINDOWS)
			SetupDebugMessages();
#endif
		}
		
		/* Create GLFW Window and screen related buffers */
		CreateSurface();

		if (!m_InitializedOnce)
		{
			PickPhysicalDevice();
			CreateLogicalDevice();

			/* Initialize Renderers, create context (necessary for swapchain image format selection) */
			m_pRenderer2D = std::unique_ptr<VkRenderer2D>(
				new VkRenderer2D(*this)
			);

			m_pRenderer3D = std::unique_ptr<VkRenderer3D>(
				new VkRenderer3D()
			);
		}
		
		m_pRenderer2D->CreateContext();
		
		/* Create swap chain and image views */
		SwapchainCtx.Init();
		
		if (!m_InitializedOnce)
		{
			/* Create screen render pass, graphics pipeline (draw pass) */
			/* SPECIFIED BY USER */

			/* Create command pool, command buffers and sync objects (command engine) */
			m_ScreenCommandEngine = new VkScreenCommandEngine();
			m_CommandEngines.emplace("Screen", m_ScreenCommandEngine);

			vk(GetPhysicalDeviceFeatures, PhysicalDevice, &Features);

			m_InitializedOnce = true;
		}

		for (auto it1 : m_CommandEngines)
		{
			it1.second->Init();
		}
		
		/* Create 2D renderer surface */
		m_pRenderer2D->CreateSurface();

		return RenderAPI::Init();
	}

	bool VkRenderAPI::Deinit()
	{
		if (!m_bInitialized)
			return false;

		vk(DeviceWaitIdle, Device);

		for (auto it1 : m_CommandEngines)
		{
			it1.second->Deinit();
		}

		m_pRenderer2D->Deinit();
		// m_pRenderer3D->Deinit();

		SwapchainCtx.Deinit();

		if (Surface)
		{
			vk(DestroySurfaceKHR, Instance, Surface, nullptr);
			Surface = nullptr;
		}

		return RenderAPI::Deinit();
	}

	void VkRenderAPI::CreateInstance()
	{
#if defined(CB_DEBUG) && defined(CB_PLATFORM_WINDOWS)
		if (!CheckValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");
		}
#endif
		uint32_t instanceVersion = VK_MAKE_VERSION(1, 0, 0);
		
		auto res = vk(EnumerateInstanceVersion, &instanceVersion);
		Verify(res);

		uint32_t apiVersion = VK_MAKE_VERSION(1, 0, 0);
		if (instanceVersion >= VK_MAKE_VERSION(1, 1, 0)) {
			// If the instance version is 1.0 we must have the apiVersion also be 1.0. However, if the
			// instance version is 1.1 or higher, we can set the apiVersion to be whatever the highest
			// api we may use in skia (technically it can be arbitrary). So for now we set it to 1.1
			// since that is the highest vulkan version.
			apiVersion = VK_MAKE_VERSION(1, 1, 0);
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Engine";
		appInfo.applicationVersion = 0;
		appInfo.pEngineName = "Engine";
		appInfo.engineVersion = 0;
		appInfo.apiVersion = apiVersion;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

#if defined(CB_DEBUG) && defined(CB_PLATFORM_WINDOWS)
			createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
			createInfo.ppEnabledLayerNames = ValidationLayers.data();

			PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
#else
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
#endif

		res = vk(CreateInstance, &createInfo, nullptr, &Instance);
		Verify(res);
	}
	
	std::vector<const char*> VkRenderAPI::GetRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#if defined(CB_DEBUG) && defined(CB_PLATFORM_WINDOWS)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

		return extensions;
	}
	
#if defined(CB_DEBUG) && defined(CB_PLATFORM_WINDOWS)
	void VkRenderAPI::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}
	
	bool VkRenderAPI::CheckValidationLayerSupport() {
		uint32_t layerCount;
		vk(EnumerateInstanceLayerProperties, &layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vk(EnumerateInstanceLayerProperties, &layerCount, availableLayers.data());

		for (const char* layerName : ValidationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}
	
	void VkRenderAPI::SetupDebugMessages()
	{
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        PopulateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(Instance, &createInfo, nullptr, &DebugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
	}

	VkResult VkRenderAPI::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vk(GetInstanceProcAddr, instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
#endif

	void VkRenderAPI::CreateSurface()
	{
		const auto res = glfwCreateWindowSurface(Instance,
			static_cast<GLFWwindow*>(m_pRenderContext->GetWindow().GetWindow()),
			nullptr,
			&Surface
		);
		Verify(res);
	}

	void VkRenderAPI::PickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vk(EnumeratePhysicalDevices, Instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vk(EnumeratePhysicalDevices, Instance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			if (IsDeviceSuitable(device)) {
				PhysicalDevice = device;
				break;
			}
		}

		if (PhysicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}
	}

	void VkRenderAPI::CreateLogicalDevice() {
		QueueFamilyIndices indices = FindQueueFamilies(PhysicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.GraphicsFamily.value(), indices.PresentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		vk(GetPhysicalDeviceFeatures, PhysicalDevice, &deviceFeatures);

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = DeviceExtensions.data();

#if defined(CB_DEBUG) && defined(CB_PLATFORM_WINDOWS)
			createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
			createInfo.ppEnabledLayerNames = ValidationLayers.data();
#else
			createInfo.enabledLayerCount = 0;
#endif

		const auto res = vk(CreateDevice, PhysicalDevice, &createInfo, nullptr, &Device);
		Verify(res);

		vk(GetDeviceQueue, Device, indices.GraphicsFamily.value(), 0, &GraphicsQueue);
		vk(GetDeviceQueue, Device, indices.PresentFamily.value(), 0, &PresentQueue);
	}

	bool VkRenderAPI::IsDeviceSuitable(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices = FindQueueFamilies(device);

		const bool extensionsSupported = CheckDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
			VkSwapchainContext::SupportDetails swapChainSupport = SwapchainCtx.QuerySupport(device);
			swapChainAdequate = !swapChainSupport.Formats.empty() && !swapChainSupport.PresentModes.empty();
		}

		return indices.isComplete() && extensionsSupported && swapChainAdequate;
	}

	bool VkRenderAPI::CheckDeviceExtensionSupport(VkPhysicalDevice device) const {
		uint32_t extensionCount;
		vk(EnumerateDeviceExtensionProperties, device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vk(EnumerateDeviceExtensionProperties, device, nullptr, &extensionCount, availableExtensions.data());

		std::set<String> requiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}
}
