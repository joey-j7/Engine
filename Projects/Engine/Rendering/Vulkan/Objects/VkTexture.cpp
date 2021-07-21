#include "pch.h"

#include "VkTexture.h"
#include "Rendering/Vulkan/VkRenderAPI.h"

namespace Engine
{
	VkImageViewCreateInfo VkTexture::m_DefaultCreateInfo = {
		VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		nullptr,
		VK_IMAGE_VIEW_CREATE_FRAGMENT_DENSITY_MAP_DYNAMIC_BIT_EXT,
		VkImage(),
		VK_IMAGE_VIEW_TYPE_2D,
		VK_FORMAT_UNDEFINED,
		{
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY,
			VK_COMPONENT_SWIZZLE_IDENTITY
		},
		{
			VK_IMAGE_ASPECT_COLOR_BIT,
			0,
			1,
			0,
			1
		}
	};

	VkTexture::VkTexture(const std::string& sName) : Texture(sName),
		m_API(VkRenderAPI::Get())
	{
		m_CreateInfo = m_DefaultCreateInfo;
	}

	VkTexture::VkTexture(const VkImage& Image, const VkImageViewCreateInfo& CreateInfo = m_DefaultCreateInfo, const std::string& sName = "Texture") : Texture(sName),                                                                                                                                                         m_API(VkRenderAPI::Get())
	{
		Init(Image, CreateInfo);
	}

	VkTexture::~VkTexture()
	{
		if (!m_Initialized)
			return;

		vk(DestroyImageView, m_API.Device, m_View, nullptr);
		vk(DestroyImage, m_API.Device, m_Image, nullptr);
	}

	void VkTexture::SetLayout(
		VkCommandBuffer CommandBuffer,
		VkImageLayout NewLayout,
		VkImageAspectFlags AspectMask
	)
	{
		if (m_Layout == NewLayout)
			return;
		
		// Setup the image memory barrier
		VkImageMemoryBarrier Barrier = {};
		Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		Barrier.srcAccessMask = GetAccessFlag(m_Layout);
		Barrier.dstAccessMask = GetAccessFlag(NewLayout);
		Barrier.oldLayout = m_Layout;
		Barrier.newLayout = NewLayout;

		/* VkRenderAPI::QueueFamilyIndices queueIndices = m_API.FindQueueFamilies(m_API.PhysicalDevice); 

		Barrier.srcQueueFamilyIndex = queueIndices.GraphicsFamily.value();
		Barrier.dstQueueFamilyIndex = queueIndices.PresentFamily.value();*/

		Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		
		Barrier.image = GetImage();
		Barrier.subresourceRange.aspectMask = AspectMask;
		Barrier.subresourceRange.baseMipLevel = 0;
		Barrier.subresourceRange.levelCount = 1;
		Barrier.subresourceRange.baseArrayLayer = 0;
		Barrier.subresourceRange.layerCount = 1;

		// Pipeline stage flags
		const VkPipelineStageFlags SrcStages = m_API.GetPipelineStageFlags(m_Layout);
		const VkPipelineStageFlags DestStages = m_API.GetPipelineStageFlags(NewLayout);

		// Setup the pipeline barrier
		vk(CmdPipelineBarrier, CommandBuffer, SrcStages, DestStages, 0, 0, NULL, 0, NULL, 1, &Barrier);

		m_Layout = NewLayout;
	}

	VkAccessFlags VkTexture::GetAccessFlag(const VkImageLayout Layout)
	{
		switch (Layout) {
		case VK_IMAGE_LAYOUT_UNDEFINED:
			return 0;
		case VK_IMAGE_LAYOUT_GENERAL:
			return VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
				VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_TRANSFER_READ_BIT |
				VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_HOST_WRITE_BIT |
				VK_ACCESS_HOST_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
				VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
				VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
				VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
		case VK_IMAGE_LAYOUT_PREINITIALIZED:
			return VK_ACCESS_HOST_WRITE_BIT;
		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
			return VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
				VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
			return VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
			return VK_ACCESS_TRANSFER_READ_BIT;
		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
			return VK_ACCESS_TRANSFER_WRITE_BIT;
		case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
			return 0;
		}
		return 0;
	}

	void VkTexture::Init(const VkImage& Image)
	{
		if (m_Initialized)
			return;

		m_Image = Image;
		m_CreateInfo.image = m_Image;
		
		const VkResult err = vk(CreateImageView, m_API.Device, &m_CreateInfo, nullptr, &m_View);
		VkRenderAPI::Verify(err);

		m_Initialized = true;
	}

	void VkTexture::Init(const VkImage& Image, const VkImageViewCreateInfo& CreateInfo)
	{
		if (m_Initialized)
			return;
		
		m_CreateInfo = CreateInfo;
		Init(Image);
	}

	void VkTexture::Deinit()
	{
		m_Initialized = false;
	}
}
