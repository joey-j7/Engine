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

		void Init();

		virtual void Swap() = 0;
		virtual void Present() = 0;
		
	protected:
		virtual void CreateContext() = 0;
		virtual void CreateSurface() = 0;
		
		sk_sp<GrDirectContext> m_Context = nullptr;
		
		sk_sp<SkSurface> m_Surface = nullptr;
		SkCanvas* m_Canvas = nullptr;

		bool m_Initialized = false;
	};
}
