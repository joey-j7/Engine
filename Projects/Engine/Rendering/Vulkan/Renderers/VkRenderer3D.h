#pragma once

#include "Rendering/Renderers/Renderer3D.h"

namespace Engine
{
	class Engine_API VkRenderer3D : public Renderer3D
	{
	public:
		VkRenderer3D() {};
		virtual ~VkRenderer3D() {};
	};
}