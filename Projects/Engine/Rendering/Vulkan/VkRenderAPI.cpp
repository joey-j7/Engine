#include "pch.h"

#include "VkRenderAPI.h"

#include "VkCommandEngine.h"
#include "Files/VkShaderFile.h"

#include "Renderers/2D/VkRenderer2D.h"
#include "Rendering/RenderContext.h"

#include <set>

#include "Passes/VkDrawPass.h"

#ifdef CB_DEBUG && defined(CB_PLATFORM_WINDOWS)
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
	const std::string s = std::string("[VULKAN] ") + pCallbackData->pMessage;
	
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
		s_Instance = this;

		// m_pDatabase->Add<VkModelFile>({ "fbx", "obj", "vox" });
		// m_pDatabase->Add<VkTextureFile>({ "png", "jpg", "bmp" });
		FileDatabase::Get().Add<VkShaderFile>({ "vspv" });

		Init();
	}

	VkRenderAPI::~VkRenderAPI()
	{
		VkResult err = vk(DeviceWaitIdle, Device);
		Verify(err);

		m_pRenderer2D.reset();
		
		for (auto& it : m_CommandEngines)
		{
			delete it.second;
		}

		SwapchainCtx.Deinit();

		vk(DestroySurfaceKHR, Instance, Surface, nullptr);
	}

	bool VkRenderAPI::Swap()
	{
		m_ScreenCommandEngine->WaitForGPU();
		
		VkResult res = vk(
			AcquireNextImageKHR, 
			Device,
			SwapchainCtx.Swapchain,
			UINT64_MAX,
			m_ScreenCommandEngine->GetImageAcquiredSemaphore(),
			VK_NULL_HANDLE,
			&SwapchainCtx.SemaphoreIndex
		);
		
		if (res == VK_ERROR_OUT_OF_DATE_KHR) {
			Reset();
			return false;
		}
		
		if (res != VK_SUBOPTIMAL_KHR)
		{
			Verify(res);
		}
		
		m_pRenderer2D->Swap();
		return true;
	}

	void VkRenderAPI::Present()
	{
		m_pRenderer2D->Present();

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		VkSemaphore signalSemaphores[] = { m_ScreenCommandEngine->GetRenderCompleteSemaphore() };
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { SwapchainCtx.Swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &SwapchainCtx.SemaphoreIndex;

		const auto res = vk(QueuePresentKHR, PresentQueue, &presentInfo);

		if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
		{
			Reset();
		}
		else
		{
			Verify(res);
		}

		SwapchainCtx.FrameIndex = (SwapchainCtx.FrameIndex + 1) % SwapchainCtx.FrameCount;
	}

	void VkRenderAPI::Suspend()
	{
		
	}

	void VkRenderAPI::Resume()
	{
		
	}

	CommandEngine* VkRenderAPI::GetCommandEngine(const std::string& sName)
	{
		auto it = m_CommandEngines.find(sName);

		if (it == m_CommandEngines.end())
			return nullptr;

		return it->second;
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

		if (!m_InitializedOnce)
		{
			m_sName = "Vulkan";

			// Setup Vulkan
			CB_CORE_ASSERT(glfwVulkanSupported(), "GLFW: Vulkan is not supported by the system");

			CreateInstance();

#if defined(CB_DEBUG) && defined(CB_PLATFORM_WINDOWS)
			SetupDebugMessages();
#endif

			/* Create GLFW Window and screen related buffers */
			CreateSurface();

			PickPhysicalDevice();
			CreateLogicalDevice();

			/* Initialize Renderers, create context (necessary for swapchain image format selection) */
			m_pRenderer2D = std::unique_ptr<VkRenderer2D>(
				new VkRenderer2D(*this)
			);
		}
		
		m_pRenderer2D->CreateContext();
		
		/* Create swap chain and image views */
		SwapchainCtx.Init();
		
		for (auto it1 : m_CommandEngines)
		{
			for (auto it2 : it1.second->GetVkPasses())
			{
				it2->Init();
			}

			it1.second->ResetSemaphoreFence();
		}

		if (!m_InitializedOnce)
		{
			/* Create screen render pass, graphics pipeline (draw pass) */
			/* SPECIFIED BY USER */

			/* Create command pool, command buffers and sync objects (command engine) */
			m_CommandEngines.emplace("Screen", new VkCommandEngine(CommandEngine::E_DIRECT, "Screen"));
			m_ScreenCommandEngine = m_CommandEngines["Screen"];

			vk(GetPhysicalDeviceFeatures, PhysicalDevice, &Features);

			m_InitializedOnce = true;
		}
		
		/* Create 2D renderer surface */
		m_pRenderer2D->CreateSurface();

		return RenderAPI::Init();
	}

	bool VkRenderAPI::Deinit()
	{
		if (!m_bInitialized)
			return false;
		
		for (auto it1 : m_CommandEngines)
		{
			it1.second->WaitForGPU();
			
			for (auto it2 : it1.second->GetVkPasses())
			{
				it2->Deinit();
			}
		}

		m_pRenderer2D->Deinit();
		SwapchainCtx.Deinit();
		
		return RenderAPI::Deinit();
	}

	void VkRenderAPI::CreateInstance()
	{
#if defined(CB_DEBUG) && defined(CB_PLATFORM_WINDOWS)
		if (!CheckValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");
		}
#endif

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "App";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "The Nest";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

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

		const auto res = vk(CreateInstance, &createInfo, nullptr, &Instance);
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

		std::set<std::string> requiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}
}
