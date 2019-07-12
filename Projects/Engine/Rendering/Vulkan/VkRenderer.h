#pragma once

#if CB_RENDERING_API == CB_RENDERER_VULKAN

#include "Rendering/RenderAPI.h"

namespace Engine {
	class Engine_API VkRenderer : public RenderAPI
	{
	public:
		VkRenderer(const std::shared_ptr<RenderContextData>& contextData);
		~VkRenderer() override;
		virtual void Clear() override;

		virtual void Verify(int err) override
		{
			if (err == 0) return;
			else if (err < 0) { CB_CORE_ASSERT(false, "VkResult %d\n", err); }
			else { CB_CORE_ERROR("VkResult %d\n", err); }
		}

	private:
		virtual void Init();
	};
}

#endif