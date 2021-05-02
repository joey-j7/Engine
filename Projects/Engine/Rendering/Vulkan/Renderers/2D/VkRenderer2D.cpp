#include "pch.h"
#include "VkRenderer2D.h"

#include <include/gpu/vk/GrVkExtensions.h>
#include <include/core/SkFont.h>
#include <include/core/SkSurface.h>

#include "Platform/Window.h"
#include "Rendering/Vulkan/VkRenderAPI.h"

#include "Engine/Application.h"

namespace Engine
{
	VkRenderer2D::VkRenderer2D(VkRenderAPI& API) : Renderer2D()
	{
		m_pAPI = &API;
	}

	VkRenderer2D::~VkRenderer2D()
	{	
		m_Surfaces.clear();
		m_Context.reset();

		m_Surface = nullptr;
		
		Image.reset();
	}

	void VkRenderer2D::Swap()
	{
		m_Surface = m_Surfaces[m_pAPI->SwapchainCtx.FrameIndex];
		m_Canvas = m_Surface->getCanvas();

		m_Canvas->clear(SK_ColorWHITE);
	}

	void VkRenderer2D::Present()
	{
		auto& Window = Application::Get().GetRenderContext().GetWindow();

		SkPaint paint;
		paint.setColor(SK_ColorRED);

		m_Canvas->save();

		const float scale = Window.GetScale();
		m_Canvas->scale(scale, scale);
		
		// Draw a rectangle with red paint
		float t = static_cast<float>(DeltaTime::GetTime());
		
		paint.setAntiAlias(true);
		int r = 0.f + glm::abs(glm::sin(t)) * 30;
		SkScalar radii[] = { r, r, r, r, r, r, r, r };
		SkMatrix translation;

		float a = 360.f;
		translation.postRotate(glm::mod(t * 90.f, a), 100, 100);
		
		SkPath path;
		path.addRoundRect({ 50, 50, 150, 150 }, radii);
		path.transform(translation);
		
		m_Canvas->drawPath(path, paint);

		// Draw a message with a nice black paint
		SkFont font;
		font.setSubpixel(true);
		font.setSize(60);
		paint.setColor(SK_ColorBLACK);

		static const char message[] = "Hello World\0";

		// Translate and rotate
		m_Canvas->translate(Window.GetWidth() / scale * 0.5f, Window.GetHeight() / scale * 0.5f);

		if (!Image)
		{
			uint32_t length = 0;
			char* data = FileLoader::Read("image.png", length, FileLoader::Type::E_CONTENT);
			Image = SkImage::MakeFromEncoded(SkData::MakeFromMalloc(data, length));
		}
		
		if (Image)
		{
			m_Canvas->drawImage(Image, 0, 0, SkSamplingOptions(SkFilterMode::kLinear));
		}

		// Draw the text
		m_Canvas->drawString(message, 0, 0, font, paint);

		m_Canvas->restore();
		m_Canvas->save();

		m_Canvas->scale(scale, scale);

		// Draw the text
		font.setSize(16);

		const std::string fps = std::to_string(DeltaTime::GetFPS()) + "FPS " + std::to_string(DeltaTime::Get()) + "ms " + std::to_string(scale) + "scale";
		m_Canvas->drawString(fps.c_str(), 50, 100, font, paint);

		m_Canvas->restore();

		m_Context->flushAndSubmit();
	}

	void VkRenderer2D::CreateContext()
	{
		if (!CreateSkiaBackendContext(m_VkContext)) {
			CB_CORE_ERROR("Could not create Skia context");
			return;
		}

		m_Context = GrDirectContext::MakeVulkan(m_VkContext);
	}

	void VkRenderer2D::CreateSurface()
	{
		const std::vector<VkImage>& images = m_pAPI->SwapchainCtx.Images;

		if (images.size() == 0) {
			CB_CORE_ERROR("No swapchain images, failed to create Skia surfaces");
			return;
		}

		const auto& Extents = m_pAPI->SwapchainCtx.CreateInfo.imageExtent;
		const SkISize surface_size = SkISize::Make(Extents.width, Extents.height);

		// Populate the surface.
		if (!m_Context || m_pAPI->SwapchainCtx.CreateInfo.imageFormat == kUnknown_SkColorType) {
			CB_CORE_ERROR("Failed to create Skia surfaces");
			return;
		}

		GrVkImageInfo image_info;
		image_info.fImageTiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.fImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.fFormat = m_pAPI->SwapchainCtx.CreateInfo.imageFormat;
		image_info.fImageUsageFlags = m_pAPI->SwapchainCtx.CreateInfo.imageUsage;
		image_info.fSampleCount = 1;
		image_info.fLevelCount = 1;

		const auto& desired = DesiredFormatInfos();
		int index = 0;

		for (uint32_t i = 0; i < desired.size(); i++)
		{
			if (desired[i].format_ == image_info.fFormat)
			{
				index = i;
				break;
			}
		}
		
		SkSurfaceProps props(0, kUnknown_SkPixelGeometry);
		
		for (const VkImage& image : images) {
			image_info.fImage = image;

			// TODO(chinmaygarde): Setup the stencil buffeVkImageUsageFlagsr and the sampleCnt.
			GrBackendRenderTarget backend_render_target(surface_size.fWidth, surface_size.fHeight, 0, image_info);

			sk_sp<SkSurface> surface = SkSurface::MakeFromBackendRenderTarget(
				m_Context.get(),							// context
				backend_render_target,						// backend render target
				kTopLeft_GrSurfaceOrigin,					// origin
				desired[index].color_type_,                 // color type
				std::move(desired[index].color_space_),     // color space
				&props										// surface properties
			);

			if (surface == nullptr) {
				CB_CORE_ERROR("Failed to create Skia surface");
				return;
			}

			m_Surfaces.emplace_back(std::move(surface));
		}

		if (images.size() != m_Surfaces.size())
		{
			CB_CORE_WARN("Inconsistent surface vector size");
		}

		m_Surface = m_Surfaces.front();
		m_Canvas = m_Surface->getCanvas();
	}

	bool VkRenderer2D::CreateSkiaBackendContext(GrVkBackendContext& context) {
		GrVkGetProc getProc = [](const char* proc_name, VkInstance instance, VkDevice device) {
			return device ? vkGetDeviceProcAddr(device, proc_name) : vkGetInstanceProcAddr(instance, proc_name);
		};

		uint32_t skia_features = 0;
		if (!GetPhysicalDeviceFeaturesSkia(&skia_features)) {
			return false;
		}

		context.fInstance = m_pAPI->Instance;
		context.fPhysicalDevice = m_pAPI->PhysicalDevice;
		context.fDevice = m_pAPI->Device;
		context.fQueue = m_pAPI->Queue;
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
		
		context.fFeatures = skia_features;
		context.fGetProc = std::move(getProc);
		context.fOwnsInstanceAndDevice = false;

		// The memory_requirements_2 extension is required on Fuchsia as the AMD
		// memory allocator used by Skia benefit from it.
		const char* device_extensions[] = {
			VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
		};
		GrVkExtensions vk_extensions;
		vk_extensions.init(context.fGetProc, context.fInstance,
			context.fPhysicalDevice, 0, nullptr,
			glm::countof(device_extensions), device_extensions);
		context.fVkExtensions = &vk_extensions;
		
		return true;
	}

	bool VkRenderer2D::GetPhysicalDeviceFeaturesSkia(uint32_t* sk_features) const {
		if (sk_features == nullptr) {
			return false;
		}

		const VkPhysicalDeviceFeatures features = m_pAPI->Features;
		uint32_t flags = 0;

		if (features.geometryShader) {
			flags |= kGeometryShader_GrVkFeatureFlag;
		}
		if (features.dualSrcBlend) {
			flags |= kDualSrcBlend_GrVkFeatureFlag;
		}
		if (features.sampleRateShading) {
			flags |= kSampleRateShading_GrVkFeatureFlag;
		}

		*sk_features = flags;
		return true;
	}
}
