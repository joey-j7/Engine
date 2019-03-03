#pragma once

#if CB_RENDERING_API == CB_RENDERER_OPENGL

#include "Engine/Renderer.h"

namespace Engine {
	class GLRenderer : public Renderer
	{
	public:
		virtual void Clear() override;
	};
}

#endif