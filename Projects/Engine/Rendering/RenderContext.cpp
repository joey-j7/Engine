#include "pch.h"
#include "RenderContext.h"

#include "Engine/Application.h"

#include "Vulkan/VkRenderAPI.h"
#include "OpenGL/GLRenderAPI.h"

namespace Engine
{
	class VkRenderAPI;

	typedef VkRenderAPI* (__stdcall* CREATEAPI)(RenderContext&);

	RenderAPI::Type RenderContext::m_APIType = RenderAPI::E_VULKAN;

	void RenderContext::Init()
	{
		if (m_Initialized)
			return;
		
		m_pWindow = std::unique_ptr<Window>(Window::Create());
		m_pWindow->SetEventCallback(std::bind(&Application::Call, &Application::Get(), std::placeholders::_1));

		if (!glfwVulkanSupported())
		{
			m_APIType = RenderAPI::E_OPENGL;
		}

		switch(m_APIType)
		{
		case RenderAPI::E_VULKAN:
			m_pAPI = std::unique_ptr<RenderAPI>(new VkRenderAPI(*this));
			break;
		default:
			m_pAPI = std::unique_ptr<RenderAPI>(new GLRenderAPI(*this));
			break;
		}

		m_Initialized = true;
	}

	void RenderContext::SetAPIType(RenderAPI::Type type)
	{
		if (m_APIType == type)
			return;

		CB_CORE_ASSERT(false, "Not implemented");

		m_APIType = type;
	}
}
