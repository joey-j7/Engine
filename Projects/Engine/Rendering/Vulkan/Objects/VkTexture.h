#pragma once

#include "Rendering/Objects/Texture.h"
#include <vulkan/vulkan.h>

namespace Engine
{
	class Engine_API VkTexture : public Texture
	{
	public:
		VkTexture(const VkImage& image, const std::string& sName = "Unnamed Texture");
		VkTexture(const std::string& sName = "Unnamed Texture");

		const VkImage& GetImage() const { return m_Image; }
		const VkImageView& GetView() const { return m_View; }

		virtual ~VkTexture();

	private:
		void Init();

		VkDevice* m_pDevice = nullptr;

		VkImage m_Image;
		VkImageView m_View;
	};
}