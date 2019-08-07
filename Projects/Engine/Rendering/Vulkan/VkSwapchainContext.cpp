#include "pch.h"

#include "VkSwapchainContext.h"

#include "Rendering/RenderContext.h"
#include "VkRenderAPI.h"

VkSwapchainContext::~VkSwapchainContext()
{
	Deinit();
}

void VkSwapchainContext::Init(Engine::VkRenderAPI& api)
{
	if (Initialized)
		return;

	Engine::RenderContext& renderContext = api.GetRenderContext();
	auto& window = renderContext.GetWindow();

	// Query swap chain support
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(api.PhysicalDevice, api.Surface, &Capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(api.PhysicalDevice, api.Surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		Formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(api.PhysicalDevice, api.Surface, &formatCount, Formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(api.PhysicalDevice, api.Surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		PresentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(api.PhysicalDevice, api.Surface, &presentModeCount, PresentModes.data());
	}

	// Get swap chain image count
	MinImageCount = Capabilities.minImageCount + 1;
	if (Capabilities.maxImageCount > 0 && MinImageCount > Capabilities.maxImageCount)
	{
		MinImageCount = Capabilities.maxImageCount;
	}

	// Choose swap chain format
	VkSurfaceFormatKHR surfaceFormat = Formats[0];

	for (const auto& availableFormat : Formats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			surfaceFormat = availableFormat;
			break;
		}
	}

	// Choose swap chain present mode
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
	VkPresentModeKHR presentMode = bestMode;

	for (const auto& availablePresentMode : PresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			presentMode = availablePresentMode;
			break;
		}
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
		{
			bestMode = availablePresentMode;
		}
	}

	// Choose swap extent
	VkExtent2D extent = Capabilities.currentExtent;
	if (Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		extent = Capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = { window.GetWidth(), window.GetHeight() };

		actualExtent.width = std::max(Capabilities.minImageExtent.width, std::min(Capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(Capabilities.minImageExtent.height, std::min(Capabilities.maxImageExtent.height, actualExtent.height));

		extent = actualExtent;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = api.Surface;

	createInfo.minImageCount = MinImageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// Find queue families
	uint32_t graphicsFamily = UINT_MAX;
	uint32_t presentFamily = UINT_MAX;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(api.PhysicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(api.PhysicalDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(api.PhysicalDevice, i, api.Surface, &presentSupport);

		if (queueFamily.queueCount > 0 && presentSupport)
		{
			presentFamily = i;
		}

		if (graphicsFamily != UINT_MAX && presentFamily != UINT_MAX)
		{
			break;
		}

		i++;
	}

	graphicsFamily = graphicsFamily == UINT_MAX ? 0 : graphicsFamily;
	presentFamily = presentFamily == UINT_MAX ? 0 : presentFamily;

	/* Create queues */
	vkGetDeviceQueue(api.Device, graphicsFamily, 0, &GraphicsQueue);
	vkGetDeviceQueue(api.Device, presentFamily, 0, &PresentQueue);

	/* Create swap chain */
	uint32_t queueFamilyIndices[] = { graphicsFamily, presentFamily };

	if (graphicsFamily != presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = Capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	const VkResult err = vkCreateSwapchainKHR(api.Device, &createInfo, api.Allocator, &Swapchain);
	Engine::VkRenderAPI::Verify(err);

	vkGetSwapchainImagesKHR(api.Device, Swapchain, &FrameCount, nullptr);
	Images.resize(FrameCount);
	Views.resize(FrameCount);
	vkGetSwapchainImagesKHR(api.Device, Swapchain, &FrameCount, Images.data());

	ImageFormat = surfaceFormat.format;
	Extent = extent;

	// Create views
	VkImageViewCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageCreateInfo.format = ImageFormat;
	imageCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	imageCreateInfo.subresourceRange.baseMipLevel = 0;
	imageCreateInfo.subresourceRange.levelCount = 1;
	imageCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageCreateInfo.subresourceRange.layerCount = 1;

	for (uint32_t i = 0; i < FrameCount; ++i)
	{
		imageCreateInfo.image = Images[i];

		VkResult err = vkCreateImageView(api.Device, &imageCreateInfo, nullptr, &Views[i]);
		api.Verify(err);
	}

	Initialized = true;
}

void VkSwapchainContext::Deinit()
{
	if (!Initialized)
		return;

	auto& api = Engine::VkRenderAPI::Get();

	for (auto view : Views)
	{
		vkDestroyImageView(api.Device, view, nullptr);
	}

	vkDestroySwapchainKHR(api.Device, Swapchain, nullptr);

	Initialized = false;
}
