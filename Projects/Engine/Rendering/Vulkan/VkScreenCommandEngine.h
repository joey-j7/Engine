#pragma once

#include "VkCommandEngine.h"

struct RenderContextData;

namespace Engine
{
	class VkRenderAPI;

	class Engine_API VkScreenCommandEngine : public VkCommandEngine
	{
	public:
		VkScreenCommandEngine();

		virtual void Init() override;
		
		bool Swap();
		void Present();
	};
}
