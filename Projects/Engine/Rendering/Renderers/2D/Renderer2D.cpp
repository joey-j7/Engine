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
		Deinit();
	}

	void Renderer2D::Deinit()
	{
		if (!m_Context)
			return;

		m_Surface.reset();
		m_Canvas = nullptr;

		m_Context->abandonContext();
		m_Context.reset();
	}
}
