#include "pch.h"

#include "Engine/Application.h"
#include "RenderContext.h"

#include "Engine/Events/Event.h"

namespace Engine
{
	void RenderContext::Init()
	{
		if (m_Initialized)
			return;

		m_ContextData = std::make_shared<RenderContextData>();

		m_Window = std::unique_ptr<Window>(Window::Create(m_ContextData));
		m_Renderer = std::unique_ptr<Renderer>(Renderer::Create(m_ContextData));

		m_Shaders = std::unique_ptr<ResourceManager<ShaderResource>>(new ResourceManager<ShaderResource>());
		m_Textures = std::unique_ptr<ResourceManager<TextureResource>>(new ResourceManager<TextureResource>());
		m_Models = std::unique_ptr<ResourceManager<ModelResource>>(new ResourceManager<ModelResource>());

		m_Initialized = true;
	}
}