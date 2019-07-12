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

		m_pContextData = std::make_shared<RenderContextData>();

		m_pWindow = std::unique_ptr<Window>(Window::Create(m_pContextData));
		m_pAPI = std::unique_ptr<RenderAPI>(RenderAPI::Create(m_pContextData));

		m_Initialized = true;
	}
}