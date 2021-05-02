#include "pch.h"

#include "VkTexture.h"

#include "../VkRenderAPI.h"

namespace Engine
{
	VkTexture::VkTexture(const VkImage& image, const std::string& sName) : Texture(sName)
	{
		m_Image = image;
		Init();
	}

	VkTexture::VkTexture(const std::string& sName) : Texture(sName)
	{
		CB_CORE_ASSERT(false, "No implementation for image creation!");
		Init();
	}

	VkTexture::~VkTexture()
	{
		vkDestroyImageView(*m_pDevice, m_View, nullptr);
		vkDestroyImage(*m_pDevice, m_Image, nullptr);
	}

	void VkTexture::Init()
	{
		m_pDevice = &VkRenderAPI::Get().Device;

		/* Create View */
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_Image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = VkRenderAPI::Get().SwapchainCtx.CreateInfo.imageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		VkResult err = vkCreateImageView(*m_pDevice, &createInfo, nullptr, &m_View);
		VkRenderAPI::Verify(err);
	}
}