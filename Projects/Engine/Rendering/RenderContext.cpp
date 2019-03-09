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

		m_Initialized = true;
	}
}