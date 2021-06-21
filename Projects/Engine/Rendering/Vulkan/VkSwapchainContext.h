#pragma once

#include "Objects/VkTexture.h"

#include <vector>
#include <optional>

namespace Engine {
	class VkRenderAPI;
	
	struct VkSwapchainContext
	{
		struct SupportDetails {
			VkSurfaceCapabilitiesKHR Capabilities;
			std::vector<VkSurfaceFormatKHR> Formats;
			std::vector<VkPresentModeKHR> PresentModes;
		};
		
		VkSwapchainContext(VkRenderAPI& api);
		~VkSwapchainContext();

		void Init();
		void Deinit();
		
		SupportDetails QuerySupport(VkPhysicalDevice device);

		VkSwapchainKHR Swapchain = VK_NULL_HANDLE;

		std::vector<VkTexture> Textures;

		uint32_t MinImageCount = 0;
		uint32_t FrameCount = 2;

		uint32_t FrameIndex = 0;
		uint32_t ImageIndex = 0;

		VkExtent2D Extent;

		VkSwapchainCreateInfoKHR CreateInfo;
		
		SupportDetails Details;

	private:
		void CheckSupport();
		
		VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		VkRenderAPI* API = nullptr;
		
		bool Initialized = false;
	};
}
