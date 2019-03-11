#include "pch.h"

#if CB_RENDERING_API == CB_RENDERER_OPENGL

#include "GLRenderer.h"
#include "glad/glad.h"

#include "Rendering/OpenGL/GLRenderDevice.h"

#include "Rendering/Resources/ModelResource.h"
#include "Rendering/Resources/TextureResource.h"
#include "Rendering/Resources/ShaderResource.h"

namespace Engine {
	const char* Renderer::m_Name = "OpenGL";

	Renderer* Renderer::Create(const std::shared_ptr<RenderContextData>& contextData)
	{
		return new GLRenderer(contextData);
	}

	GLRenderer::GLRenderer(const std::shared_ptr<RenderContextData>& contextData) : Renderer(contextData)
	{
		m_pRenderDevice = std::make_unique<GLRenderDevice>();

		m_pDatabase->Get<TextureResource>()->SetFunctions(
			[this](TextureResource* texture) { m_pRenderDevice->LoadTexture(texture); },
			[this](TextureResource* texture) { m_pRenderDevice->UnloadTexture(texture); }
		);

		m_pDatabase->Get<ShaderResource>()->SetFunctions(
			[this](ShaderResource* shader) { m_pRenderDevice->LoadShader(shader); },
			[this](ShaderResource* shader) { m_pRenderDevice->UnloadShader(shader); }
		);

		m_pDatabase->Get<ModelResource>()->SetFunctions(
			[this](ModelResource* model) { m_pRenderDevice->LoadModel(model); },
			[this](ModelResource* model) { m_pRenderDevice->UnloadModel(model); }
		);
	}

	void GLRenderer::Clear()
	{
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

#endif