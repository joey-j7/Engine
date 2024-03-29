#pragma once

#include "Rendering/Renderers/Renderer2D.h"

#include <include/gpu/vk/GrVkBackendContext.h>
#include <include/gpu/vk/GrVkExtensions.h>
#include <include/core/SkColorSpace.h>

#include <map>

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
		static std::vector<Vk2DFormatInfo> DesiredFormatInfos(const std::vector<VkSurfaceFormatKHR>& formats);

		VkRenderer2D() {};
		VkRenderer2D(VkRenderAPI& API);
		
		virtual ~VkRenderer2D();

		virtual void Swap() override;

		bool IsSupported(SkColorType color) const;

	protected:
		virtual void CreateContext() override;
		virtual void CreateSurface() override;

		virtual void Deinit() override;

		bool CreateSkiaBackendContext(GrVkBackendContext& backend_context);

		VkRenderAPI* m_pAPI = nullptr;
		GrVkBackendContext m_VkContext;

		GrVkExtensions Extensions;
		std::vector<sk_sp<SkSurface>> m_Surfaces;
		
		sk_sp<SkImage> Image;
	};
}
