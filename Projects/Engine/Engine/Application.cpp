#include "pch.h"
#include "Application.h"

#include "Platform/FileLoader.h"

#include "Rendering/RenderAPI.h"
#include "Rendering/RenderContext.h"

namespace Engine {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& sName) : LayeredObject(sName)
	{
		CB_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		m_RenderContext = std::make_shared<RenderContext>();
		m_RenderContext->Init();
		m_RenderContext->GetWindow().SetEventCallback(BIND_EVENT_FN(Call));

		FileLoader::Init();

		/*Shader::Descriptor shaderDesc;
		shaderDesc.Vertex = "default.vs.glsl";
		shaderDesc.Pixel = "default.ps.glsl";*/
		// m_RenderContext->GetAPI().GetRenderDevice().CreateShaderProgram(shaderDesc);

		m_WorldManagerLayer = new WorldManagerLayer();
		PushOverlay(m_WorldManagerLayer);

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_DeltaTime = std::unique_ptr<DeltaTime>(new DeltaTime());
	}

	Application::~Application()
	{
		// s_Instance = nullptr;
	}

	void Application::Call(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		EventDispatcher dispatcher1(e);
		dispatcher1.Dispatch<WindowMinimizeEvent>(BIND_EVENT_FN(OnWindowMinimize));

		EventDispatcher dispatcher2(e);
		dispatcher2.Dispatch<WindowFocusEvent>(BIND_EVENT_FN(OnWindowFocus));

		EventDispatcher dispatcher3(e);
		dispatcher3.Dispatch<AppPauseEvent>(BIND_EVENT_FN(OnAppPause));

		LayeredObject::Call(e);
	}

	void Application::Run()
	{
		while (m_bRunning)
		{
			m_DeltaTime->Update();
			m_RenderContext->GetAPI().Clear();

			const float fDeltaTime = m_DeltaTime->Get();

			// Update layers
			Update(fDeltaTime);

			if (m_DeltaTime->IsFixed())
			{
				// Update layers
				const float fFixedDeltaTime = m_DeltaTime->GetFixed();
				FixedUpdate(fFixedDeltaTime);
			}

			m_ImGuiLayer->Begin();
			// Update layers
			Draw(fDeltaTime);
			m_ImGuiLayer->End();

			// Update layers
			LateUpdate(fDeltaTime);

			m_RenderContext->GetWindow().OnUpdate();
		}
	}

	bool Application::OnAppPause(AppPauseEvent& e)
	{
		m_bPaused = e.IsPaused();
		return true;
	}

	bool Application::OnWindowMinimize(WindowMinimizeEvent& e)
	{
		if (!e.IsMinimized())
		{
			m_RenderContext->GetWindow().Reset();

			// Reset layer
			m_ImGuiLayer->Reset(m_LayerStack);
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
		m_bRunning = false;
		return true;
	}
}