#pragma once

#if CB_RENDERING_API == CB_RENDERER_OPENGL

#include "Engine/Renderer.h"

namespace Engine {
	class GLRenderer : public Renderer
	{
	public:
		GLRenderer(const std::shared_ptr<RenderContextData>& contextData) : Renderer(contextData) {};
		virtual void Clear() override;
	};
}

#endif