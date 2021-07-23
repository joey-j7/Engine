#pragma once

#include "Engine/Objects/Object.h"
#include "Rendering/Renderers/Renderer.h"

namespace Engine
{
	class Engine_API Renderer3D : public Renderer
	{
		friend class UIComponent;

	public:
		Renderer3D() {};
		virtual ~Renderer3D() {};

		virtual void Swap() {};
		virtual void Present() {};
	};
}
