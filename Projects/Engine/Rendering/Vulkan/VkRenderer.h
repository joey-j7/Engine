#pragma once

#if CB_RENDERING_API == CB_RENDERER_VULKAN

#include "Rendering/Renderer.h"

namespace Engine {
	class VkRenderer : public Renderer
	{
	public:
		VkRenderer(const std::shared_ptr<RenderContextData>& contextData);
		~VkRenderer() override;
		virtual void Clear() override;

		virtual void Verify(int err) override
		{
			if (err == 0) return;
			CB_CORE_ERROR("VkResult %d\n", err);
			if (err < 0) abort();
		}

	private:
		virtual void Init();
	};
}

#endif