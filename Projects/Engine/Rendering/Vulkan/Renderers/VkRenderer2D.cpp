#include "pch.h"
#include "VkRenderer2D.h"

#include <include/core/SkFont.h>
#include <include/core/SkSurface.h>
#include <src/gpu/vk/GrVkUtil.h>
#include <src/gpu/vk/GrVkCaps.h>

#include "Platform/Window.h"
#include "Rendering/Vulkan/VkRenderAPI.h"

#include "Engine/Application.h"

namespace Engine
{
	std::vector<Vk2DFormatInfo> VkRenderer2D::DesiredFormatInfos(const std::vector<VkSurfaceFormatKHR>& formats)
	{
		std::vector<Vk2DFormatInfo> list;

		for (auto& format : formats)
		{
			if (GrVkFormatIsSupported(format.format))
			{
				Vk2DFormatInfo info;
				info.format_ = format.format;

				/* Color type */
				switch (format.format)
				{
				case VK_FORMAT_B8G8R8A8_SRGB:
				case VK_FORMAT_B8G8R8A8_UNORM:
					info.color_type_ = kBGRA_8888_SkColorType;
					break;
				case VK_FORMAT_R16G16B16A16_SFLOAT:
					info.color_type_ = kRGBA_F16_SkColorType;
					break;
				case VK_FORMAT_R8G8B8A8_SRGB:
				case VK_FORMAT_R8G8B8A8_UNORM:
					info.color_type_ = kRGBA_8888_SkColorType;
					break;

					// Unsupported
				default:
					continue;
				}
				
				/* Color space */
				switch (format.colorSpace)
				{
				case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT:
				case VK_COLOR_SPACE_DISPLAY_P3_LINEAR_EXT:
				case VK_COLOR_SPACE_BT709_LINEAR_EXT:
				case VK_COLOR_SPACE_BT2020_LINEAR_EXT:
				case VK_COLOR_SPACE_ADOBERGB_LINEAR_EXT:
					info.color_space_ = SkColorSpace::MakeSRGBLinear();
					break;
				case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
				case VK_COLOR_SPACE_DISPLAY_P3_NONLINEAR_EXT:
				case VK_COLOR_SPACE_DCI_P3_NONLINEAR_EXT:
				case VK_COLOR_SPACE_BT709_NONLINEAR_EXT:
				case VK_COLOR_SPACE_ADOBERGB_NONLINEAR_EXT:
				case VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT:
				default:
					info.color_space_ = SkColorSpace::MakeSRGB();
					break;
				}
				
				list.push_back(
					info
				);
			}
		}

		return list;
	}

	VkRenderer2D::VkRenderer2D(VkRenderAPI& API) : Renderer2D()
	{
		m_pAPI = &API;
	}

	VkRenderer2D::~VkRenderer2D()
	{	
		Deinit();
		
		Image.reset();
	}

	void VkRenderer2D::Swap()
	{
		m_Surface = m_Surfaces[m_pAPI->SwapchainCtx.ImageIndex];
		m_Canvas = m_Surface->getCanvas();

		GrBackendRenderTarget backendRT = m_Surface->getBackendRenderTarget(SkSurface::kFlushRead_BackendHandleAccess);
		backendRT.setVkImageLayout(m_pAPI->SwapchainCtx.Textures[m_pAPI->SwapchainCtx.ImageIndex].GetLayout());

		m_Canvas->clear(SK_ColorWHITE);
	}
	
	bool VkRenderer2D::IsSupported(SkColorType color) const
	{
		return m_Context->colorTypeSupportedAsSurface(color);
	}

	void VkRenderer2D::CreateContext()
	{
		if (m_Context)
			return;
		
		if (!CreateSkiaBackendContext(m_VkContext)) {
			CB_CORE_ERROR("Could not create Skia context");
			return;
		}

		m_Context = GrDirectContext::MakeVulkan(m_VkContext);
	}

	void VkRenderer2D::CreateSurface()
	{
		if (m_Surfaces.size() > 0)
			return;

		const std::vector<VkTexture>& images = m_pAPI->SwapchainCtx.Textures;

		if (images.size() == 0) {
			CB_CORE_ERROR("No swapchain images, failed to create Skia surfaces");
			return;
		}
		
		const auto& extents = m_pAPI->SwapchainCtx.CreateInfo.imageExtent;
		const auto& format = m_pAPI->SwapchainCtx.CreateInfo.imageFormat;
		const auto& space = m_pAPI->SwapchainCtx.CreateInfo.imageColorSpace;
		const auto& usages = m_pAPI->SwapchainCtx.CreateInfo.imageUsage;
		const SkISize surface_size = SkISize::Make(extents.width, extents.height);

		const std::vector<VkSurfaceFormatKHR> formats = {
			{
				format,
				space
			}
		};
		const auto& desired = DesiredFormatInfos(formats);

		// Populate the surface.
		GrVkImageInfo image_info;
		image_info.fImageTiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.fImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.fFormat = format;
		image_info.fImageUsageFlags = usages;
		image_info.fSampleCount = 1;
		image_info.fLevelCount = 1;
		
		for (const VkTexture& image : images) {
			image_info.fImage = image.GetImage();

			// TODO(chinmaygarde): Setup the stencil buffer and the sampleCnt.
			GrBackendRenderTarget backend_render_target(surface_size.fWidth, surface_size.fHeight, 0, image_info);
			SkSurfaceProps props(0, kUnknown_SkPixelGeometry);

			sk_sp<SkSurface> surface = SkSurface::MakeFromBackendRenderTarget(
				m_Context.get(),                // context
				backend_render_target,     // backend render target
				kTopLeft_GrSurfaceOrigin,  // origin
				desired[0].color_type_,                 // color type
				desired[0].color_space_,     // color space
				&props                     // surface properties
			);

			if (surface == nullptr) {
				return;
			}

			m_Surfaces.emplace_back(std::move(surface));
		}

		m_Surface = m_Surfaces.front();
		m_Canvas = m_Surface->getCanvas();
	}

	void VkRenderer2D::Deinit()
	{
		if (!m_Context)
			return;

		for (auto& Surface : m_Surfaces)
		{
			Surface.reset();
		}

		m_Surfaces.clear();
		
		m_Canvas = nullptr;

		m_Context->abandonContext();
		m_Context.reset();
	}

	bool VkRenderer2D::CreateSkiaBackendContext(GrVkBackendContext& context) {
		GrVkGetProc getProc = [](const char* proc_name, VkInstance instance, VkDevice device) {
			if (device) return vk(GetDeviceProcAddr, device, proc_name);
			return vk(GetInstanceProcAddr, instance, proc_name);
		};

		context.fInstance = m_pAPI->Instance;
		context.fPhysicalDevice = m_pAPI->PhysicalDevice;
		context.fDevice = m_pAPI->Device;
		context.fQueue = m_pAPI->GetGraphicsQueue();

		context.fDeviceFeatures = &m_pAPI->Features;
		context.fGraphicsQueueIndex = 0;
		context.fMinAPIVersion = VK_API_VERSION_1_0;
		context.fMaxAPIVersion = VK_MAKE_VERSION(1, 1, 0);
		
		context.fExtensions = kKHR_surface_GrVkExtensionFlag |
			kKHR_swapchain_GrVkExtensionFlag |
#ifdef CB_PLATFORM_WINDOWS
			kKHR_win32_surface_GrVkExtensionFlag
#endif
#ifdef CB_PLATFORM_ANDROID
			kKHR_android_surface_GrVkExtensionFlag
#endif
		;
		
		context.fGetProc = std::move(getProc);
		context.fOwnsInstanceAndDevice = false;

		// The memory_requirements_2 extension is required on Fuchsia as the AMD
		// memory allocator used by Skia benefit from it.
		const char* device_extensions[] = {
			VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
		};
		
		Extensions.init(context.fGetProc, context.fInstance,
			context.fPhysicalDevice, 0, nullptr,
			static_cast<uint32_t>(glm::countof(device_extensions)), device_extensions
		);
		
		context.fVkExtensions = &Extensions;
		
		return true;
	}
}
