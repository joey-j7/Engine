#include "pch.h"

#if CB_RENDERING_API == CB_RENDERER_OPENGL

#include "GLRenderer.h"
#include <glad/glad.h>

namespace Engine {
	const char* Renderer::m_Name = "OpenGL";

	Renderer* Renderer::Create()
	{
		return new GLRenderer();
	}

	void GLRenderer::Clear()
	{
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

#endif