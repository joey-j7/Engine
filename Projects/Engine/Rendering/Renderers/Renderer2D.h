#pragma once

#include "Engine/Objects/Object.h"

#include <include/gpu/GrDirectContext.h>
#include <include/core/SkSurface.h>
#include <include/core/SkCanvas.h>

#include "Rendering/Renderers/Renderer.h"

namespace Engine
{
	class Engine_API Renderer2D : public Renderer
	{
		friend class UIElement;
		
	public:
		Renderer2D();
		virtual ~Renderer2D();

		virtual void Swap() = 0;
		virtual void Present();

		virtual void CreateContext() = 0;
		virtual void Deinit() {}
		
		virtual void CreateSurface() = 0;

		SkColorType GetSkColorType() const { return m_SkColorType; }
		SkColorSpace* GetSkColorSpace() const { return m_SkColorSpace.get(); }
		
		GrDirectContext* GetContext() const { return m_Context.get(); }
		SkCanvas* GetCanvas() const { return m_Canvas; }

	protected:
		sk_sp<GrDirectContext> m_Context = nullptr;
		sk_sp<SkSurface> m_Surface = nullptr;

		SkColorType m_SkColorType;
		sk_sp<SkColorSpace> m_SkColorSpace;
		
		SkCanvas* m_Canvas = nullptr;
		
		bool m_Initialized = false;
	};
}
