#include "pch.h"

#if CB_RENDERING_API == CB_RENDERER_OPENGL

#include "GLRenderer.h"
#include "glad/glad.h"

#include "Rendering/OpenGL/GLRenderDevice.h"

#include "Rendering/Resources/ModelResource.h"
#include "Rendering/Resources/TextureResource.h"
#include "Rendering/Resources/ShaderResource.h"

namespace Engine {
	const char* RenderAPI::m_Name = "OpenGL";

	RenderAPI* RenderAPI::Create(const std::shared_ptr<RenderContextData>& contextData)
	{
		return new GLRenderer(contextData);
	}

	GLRenderer::GLRenderer(const std::shared_ptr<RenderContextData>& contextData) : RenderAPI(contextData)
	{
		m_pDatabase = std::make_unique<GLRenderDevice>();

		m_pDatabase->Get<TextureResource>()->SetFunctions(
			[this](TextureResource* texture) { m_pDatabase->LoadTexture(texture); },
			[this](TextureResource* texture) { m_pDatabase->UnloadTexture(texture); }
		);

		m_pDatabase->Get<ShaderResource>()->SetFunctions(
			[this](ShaderResource* shader) { m_pDatabase->LoadShader(shader); },
			[this](ShaderResource* shader) { m_pDatabase->UnloadShader(shader); }
		);

		m_pDatabase->Get<ModelResource>()->SetFunctions(
			[this](ModelResource* model) { m_pDatabase->LoadModel(model); },
			[this](ModelResource* model) { m_pDatabase->UnloadModel(model); }
		);
	}

	void GLRenderer::Clear()
	{
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

#endif