#include "pch.h"

#include "VkSwapchainContext.h"

#include "Rendering/RenderContext.h"
#include "VkRenderAPI.h"
#include "Engine/Application.h"

#include <map>
#include "Renderers/2D/VkRenderer2D.h"

namespace Engine
{
	VkSwapchainContext::VkSwapchainContext(VkRenderAPI& api)
	{
		API = &api;
	}

	VkSwapchainContext::~VkSwapchainContext()
	{
		Deinit();
	}

	void VkSwapchainContext::Init()
	{
		if (Initialized)
			return;

		CheckSupport();

		auto res = vk(CreateSwapchainKHR, API->Device, &CreateInfo, nullptr, &Swapchain);
		VkRenderAPI::Verify(res);

		res = vk(GetSwapchainImagesKHR, API->Device, Swapchain, &FrameCount, nullptr);
		VkRenderAPI::Verify(res);

		std::vector<VkImage> NativeImages;
		NativeImages.resize(FrameCount);
		
		res = vk(GetSwapchainImagesKHR, API->Device, Swapchain, &FrameCount, NativeImages.data());
		VkRenderAPI::Verify(res);

		// Create textures with image views
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = CreateInfo.imageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		
		Textures.resize(FrameCount);

		for (uint32_t i = 0; i < NativeImages.size(); ++i)
		{
			Textures[i].Init(
				NativeImages[i],
				createInfo
			);

			Textures[i].SetName("Swapchain Texture " + std::to_string(i));
		}
		
		Initialized = true;
	}

	void VkSwapchainContext::Deinit()
	{
		if (!Initialized)
			return;

		// Deinit because image views should be destroyed through vkDestroySwapchainKHR
		for (VkTexture& Texture : Textures)
		{
			Texture.Deinit();
			vk(DestroyImageView, API->Device, Texture.GetView(), nullptr);
		}
		
		Textures.clear();

		vk(DestroySwapchainKHR, API->Device, Swapchain, nullptr);
		Swapchain = VK_NULL_HANDLE;

		FrameIndex = 0;
		ImageIndex = 0;

		Initialized = false;
	}
	
	VkSwapchainContext::SupportDetails VkSwapchainContext::QuerySupport(VkPhysicalDevice Device)
	{
		SupportDetails details;

		vk(GetPhysicalDeviceSurfaceCapabilitiesKHR, Device, API->Surface, &details.Capabilities);

		uint32_t formatCount;
		vk(GetPhysicalDeviceSurfaceFormatsKHR, Device, API->Surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.Formats.resize(formatCount);
			vk(GetPhysicalDeviceSurfaceFormatsKHR, Device, API->Surface, &formatCount, details.Formats.data());
		}

		uint32_t presentModeCount;
		vk(GetPhysicalDeviceSurfacePresentModesKHR, Device, API->Surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.PresentModes.resize(presentModeCount);
			vk(GetPhysicalDeviceSurfacePresentModesKHR, Device, API->Surface, &presentModeCount, details.PresentModes.data());
		}

		return details;
	}

	void VkSwapchainContext::CheckSupport()
	{
		Details = QuerySupport(API->PhysicalDevice);

		const VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(Details.Formats);
		const VkPresentModeKHR presentMode = ChoosePresentMode(Details.PresentModes);

		Extent = ChooseExtent(Details.Capabilities);

		MinImageCount = FrameCount = Details.Capabilities.minImageCount + 1;
		if (Details.Capabilities.maxImageCount > 0 && MinImageCount > Details.Capabilities.maxImageCount) {
			FrameCount = Details.Capabilities.maxImageCount;
		}

		CreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		CreateInfo.surface = API->Surface;

		CreateInfo.minImageCount = FrameCount;

		CreateInfo.imageFormat = surfaceFormat.format;
		VkTexture::m_DefaultCreateInfo.format = CreateInfo.imageFormat;
		
		CreateInfo.imageColorSpace = surfaceFormat.colorSpace;
		CreateInfo.imageExtent = Extent;
		CreateInfo.imageArrayLayers = 1;

		CreateInfo.oldSwapchain = Swapchain;

		CreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
			VK_IMAGE_USAGE_TRANSFER_DST_BIT;

		VkRenderAPI::QueueFamilyIndices queueIndices = API->FindQueueFamilies(API->PhysicalDevice);
		uint32_t queueFamilyIndices[] = { queueIndices.GraphicsFamily.value(), queueIndices.PresentFamily.value() };

		if (queueIndices.GraphicsFamily != queueIndices.PresentFamily) {
			CreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			CreateInfo.queueFamilyIndexCount = 2;
			CreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			CreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		CreateInfo.preTransform = Details.Capabilities.currentTransform;
		CreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		CreateInfo.presentMode = presentMode;
		CreateInfo.clipped = VK_TRUE;
	}

	VkSurfaceFormatKHR VkSwapchainContext::ChooseSurfaceFormat(
		const std::vector<VkSurfaceFormatKHR>& availableFormats
	)
	{
		if (availableFormats.size() == 0) {
			return VkSurfaceFormatKHR();
		}

		/* Map available formats*/
		std::map<VkFormat, VkSurfaceFormatKHR> supported_formats;
		for (uint32_t i = 0; i < availableFormats.size(); i++) {
			supported_formats[availableFormats[i].format] = availableFormats[i];
		}

		/* Get desired formats */
		const VkRenderer2D* renderer2d = static_cast<VkRenderer2D*>(API->GetRenderer2D());
		const auto format_infos = renderer2d->DesiredFormatInfos(availableFormats);
		std::vector<VkFormat> desired_formats(format_infos.size());
		for (size_t i = 0; i < format_infos.size(); ++i) {
			if (renderer2d->IsSupported(format_infos[i].color_type_))
			{
				desired_formats[i] = format_infos[i].format_;
			}
			else {
				desired_formats[i] = VK_FORMAT_UNDEFINED;
			}
		}

		// Try to find the first supported format in the list of desired formats.
		for (size_t i = 0; i < desired_formats.size(); ++i) {
			auto found = supported_formats.find(desired_formats[i]);
			if (found != supported_formats.end()) {
				return found->second;
			}
		}

		return availableFormats[0];
	}
	
	VkPresentModeKHR VkSwapchainContext::ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		const auto& window = API->GetRenderContext().GetWindow();
		const bool vsync = window.IsVSync();
		const bool tripple = window.IsTrippleBuffering();

		const VkPresentModeKHR preference =
			vsync && tripple ?
			VK_PRESENT_MODE_MAILBOX_KHR :
			vsync ?
			VK_PRESENT_MODE_FIFO_KHR :
			VK_PRESENT_MODE_IMMEDIATE_KHR
		;
	
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == preference)
			{
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VkSwapchainContext::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != UINT32_MAX) {
			return capabilities.currentExtent;
		}

		auto& window = API->GetRenderContext().GetWindow();

		VkExtent2D extent = Details.Capabilities.currentExtent;
		if (Details.Capabilities.currentExtent.width == UINT_MAX)
		{
			VkExtent2D actualExtent = { window.GetWidth(), window.GetHeight() };

			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

			extent = actualExtent;
		}

		return extent;
	}
}
