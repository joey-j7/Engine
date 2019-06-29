#include "pch.h"
#include "Application.h"
#include "Input.h"

#include "Platform/FileLoader.h"

#include "Rendering/RenderDevice.h"
#include "Rendering/Renderer.h"
#include "Rendering/RenderContext.h"

namespace Engine {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application() 
	{
		CB_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_RenderContext = std::make_shared<RenderContext>();
		m_RenderContext->Init();
		m_RenderContext->GetWindow().SetEventCallback(BIND_EVENT_FN(OnEvent));

		FileLoader::Init();

		Shader::Descriptor shaderDesc;
		shaderDesc.Vertex = "default.vs.glsl";
		shaderDesc.Pixel = "default.ps.glsl";
		// m_RenderContext->GetRenderer().GetRenderDevice().CreateShaderProgram(shaderDesc);

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_DeltaTime = std::unique_ptr<DeltaTime>(new DeltaTime());
	}

	Application::~Application()
	{
		// s_Instance = nullptr;
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		EventDispatcher dispatcher1(e);
		dispatcher1.Dispatch<WindowMinimizeEvent>(BIND_EVENT_FN(OnWindowMinimize));

		EventDispatcher dispatcher2(e);
		dispatcher2.Dispatch<WindowFocusEvent>(BIND_EVENT_FN(OnWindowFocus));

		EventDispatcher dispatcher3(e);
		dispatcher3.Dispatch<AppPauseEvent>(BIND_EVENT_FN(OnAppPause));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::Run()
	{
		while (m_Running)
		{
			if (!m_Paused)
			{
				m_DeltaTime->Update();
				m_RenderContext->GetRenderer().Clear();

				for (Layer* layer : m_LayerStack)
					layer->OnUpdate();

				m_ImGuiLayer->Begin();
				for (Layer* layer : m_LayerStack)
					layer->OnImGuiRender();
				m_ImGuiLayer->End();
			}

			m_RenderContext->GetWindow().OnUpdate();
		}
	}

	bool Application::OnAppPause(AppPauseEvent& e)
	{
		m_Paused = e.IsPaused();
		return true;
	}

	bool Application::OnWindowMinimize(WindowMinimizeEvent& e)
	{
		if (!e.IsMinimized())
		{
			m_RenderContext->GetWindow().Reset();

			// Reset layer
			m_ImGuiLayer->OnDetach();
			m_ImGuiLayer->OnAttach();
		}

		AppPauseEvent event(e.IsMinimized());
		OnAppPause(event);
		return true;
	}

	bool Application::OnWindowFocus(WindowFocusEvent& e)
	{
		AppPauseEvent event(!e.IsFocussed());
		OnAppPause(event);
		return true;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}