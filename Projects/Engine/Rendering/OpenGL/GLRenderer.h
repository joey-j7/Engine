#pragma once

#if CB_RENDERING_API == CB_RENDERER_OPENGL

#include "Rendering/Renderer.h"

namespace Engine {
	class Engine_API GLRenderer : public Renderer
	{
	public:
		GLRenderer(const std::shared_ptr<RenderContextData>& contextData);
		virtual void Clear() override;
	};
}

#endif