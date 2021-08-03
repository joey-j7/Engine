#pragma once

#include "Rendering/Objects/Texture.h"
#include <vulkan/vulkan.h>

namespace Engine
{
	class VkRenderAPI;
	
	class Engine_API VkTexture : public Texture
	{
	public:
		static VkImageViewCreateInfo m_DefaultCreateInfo;

		VkTexture(const String& sName = "Texture");
		VkTexture(const VkImage& Image, const VkImageViewCreateInfo& CreateInfo, const String & sName);
		
		virtual ~VkTexture();
		
		void SetLayout(
			VkCommandBuffer CommandBuffer,
			VkImageLayout NewLayout,
			VkImageAspectFlags AspectMask = VK_IMAGE_ASPECT_COLOR_BIT
		);

		static VkAccessFlags GetAccessFlag(const VkImageLayout Layout);
		
		const VkImage& GetImage() const { return m_Image; }
		const VkImageView& GetView() const { return m_View; }
		const VkImageLayout& GetLayout() const { return m_Layout; }

		void Init(const VkImage& Image);
		void Init(const VkImage& Image, const VkImageViewCreateInfo& CreateInfo);
		void Deinit();
		
	private:
		bool m_Initialized = false;

		VkRenderAPI& m_API;

		VkImage m_Image;
		VkImageView m_View;

		VkImageLayout m_Layout = VK_IMAGE_LAYOUT_UNDEFINED;
		
		VkImageViewCreateInfo m_CreateInfo;
	};
}
