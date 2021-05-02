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
		m_Context.reset();
		
		m_Surface = nullptr;
		m_Canvas = nullptr;
	}

	void Renderer2D::Init()
	{
		CreateContext();
		SkASSERT(m_Context);

		CreateSurface();
		SkASSERT(m_Surface);
	}
}
