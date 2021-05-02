#pragma once

#include "Rendering/Renderers/2D/Renderer2D.h"

#include <include/gpu/vk/GrVkBackendContext.h>
#include <include/core/SkColorSpace.h>

namespace Engine
{
	class VkRenderAPI;
	
	struct Vk2DFormatInfo {
		VkFormat format_;
		SkColorType color_type_;
		sk_sp<SkColorSpace> color_space_;
	};

	class Engine_API VkRenderer2D : public Renderer2D
	{
	public:
		VkRenderer2D() {};
		VkRenderer2D(VkRenderAPI& API);
		
		virtual ~VkRenderer2D();

		virtual void Swap() override;
		virtual void Present() override;

	protected:
		virtual void CreateContext() override;
		virtual void CreateSurface() override;

		bool CreateSkiaBackendContext(GrVkBackendContext& backend_context);
		bool GetPhysicalDeviceFeaturesSkia(uint32_t* sk_features) const;

		static std::vector<Vk2DFormatInfo> DesiredFormatInfos() {
			return { {VK_FORMAT_R8G8B8A8_SRGB, kRGBA_8888_SkColorType,
					 SkColorSpace::MakeSRGB()},
					{VK_FORMAT_B8G8R8A8_SRGB, kRGBA_8888_SkColorType,
					 SkColorSpace::MakeSRGB()},
					{VK_FORMAT_R16G16B16A16_SFLOAT, kRGBA_F16_SkColorType,
					 SkColorSpace::MakeSRGBLinear()},
					{VK_FORMAT_R8G8B8A8_UNORM, kRGBA_8888_SkColorType,
					 SkColorSpace::MakeSRGB()},
					{VK_FORMAT_B8G8R8A8_UNORM, kBGRA_8888_SkColorType,
					 SkColorSpace::MakeSRGB()}
			};
		}
		
		VkRenderAPI* m_pAPI = nullptr;
		GrVkBackendContext m_VkContext;

		std::vector<sk_sp<SkSurface>> m_Surfaces;
		
		sk_sp<SkImage> Image;
	};
}
