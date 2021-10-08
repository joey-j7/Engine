#include "pch.h"
#include "Renderer2D.h"

#include <include/gpu/gl/GrGLInterface.h>
#include <glad/glad.h>

#include "Engine/Application.h"

namespace Engine
{
	Renderer2D::Renderer2D()
	{

	}

	Renderer2D::~Renderer2D()
	{

	}

	void Renderer2D::Present()
	{
		for (auto& Component : m_Components)
		{
			if (!Component->IsVisible())
				continue;

			Component->BeginDraw();
			Component->Draw();
			Component->EndDraw();

			// Component->m_RenderState = Renderable::E_IDLE;
		}

		// m_Components.clear();

		m_Surface->flushAndSubmit();
	}
}
