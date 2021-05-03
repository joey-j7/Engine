#pragma once

#include "Objects/VkTexture.h"

#include <vector>
#include <memory>

namespace Engine {
	class VkRenderAPI;

	struct VkSwapchainContext
	{
		VkSwapchainContext() = default;
		~VkSwapchainContext();

		void Init(Engine::VkRenderAPI& api);
		void Deinit();

		VkSwapchainKHR Swapchain;

		std::vector<VkImage> Images;
		std::vector<VkImageView> Views;

		uint32_t MinImageCount = 0;
		uint32_t FrameCount = 2;

		uint32_t FrameIndex = 0;
		uint32_t SemaphoreIndex = 0;

		VkExtent2D Extent;

		VkQueue GraphicsQueue;
		VkQueue PresentQueue;

		VkSwapchainCreateInfoKHR CreateInfo;
		
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;

	private:
		bool Initialized = false;
	};

}