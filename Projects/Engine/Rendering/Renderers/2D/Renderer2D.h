#pragma once

#include "Engine/Objects/Object.h"

#include <include/gpu/GrDirectContext.h>
#include <include/core/SkSurface.h>
#include <include/core/SkCanvas.h>

namespace Engine
{
	class Engine_API Renderer2D : public Object
	{
	public:
		Renderer2D();
		virtual ~Renderer2D();

		virtual void Swap() = 0;
		virtual void Present() = 0;

		virtual void CreateContext() = 0;
		virtual void Deinit();
		
		virtual void CreateSurface() = 0;
		
	protected:
		sk_sp<GrDirectContext> m_Context = nullptr;
		sk_sp<SkSurface> m_Surface = nullptr;
		
		SkCanvas* m_Canvas = nullptr;

		bool m_Initialized = false;
	};
}
