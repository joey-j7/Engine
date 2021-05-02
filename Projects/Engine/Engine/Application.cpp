#include "pch.h"
#include "Application.h"

#include "Platform/FileLoader.h"

#include "Rendering/RenderAPI.h"
#include "Rendering/RenderContext.h"

#include "Rendering/CommandEngine.h"

namespace Engine
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& sName) : LayeredObject(sName)
	{
		CB_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		
		m_Database = std::make_unique<FileDatabase>();

		m_RenderContext = std::make_shared<RenderContext>();
		m_RenderContext->Init();

		FileLoader::Init();

		m_pScreenEngine = m_RenderContext->GetAPI().GetCommandEngine("Screen");

		m_WorldManagerLayer = new WorldManagerLayer();
		PushOverlay(m_WorldManagerLayer);

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
		bool b = false;
		
		while (m_bRunning)
		{
				m_RenderContext->GetAPI().Swap();

				// Reset rendering for new frame
				m_pScreenEngine->Reset();
				m_pScreenEngine->Begin();
		
			// Retrieve delta time for logic
			m_DeltaTime->Update();
			const float fDeltaTime = m_DeltaTime->Get();

			// Update layers
			Update(fDeltaTime);

			if (m_DeltaTime->IsFixed())
			{
				// Update layers
				const float fFixedDeltaTime = m_DeltaTime->GetFixed();
				FixedUpdate(fFixedDeltaTime);
			}

			// Update layers
			LateUpdate(fDeltaTime);

			// Draw layers
			Draw(fDeltaTime);

			m_pScreenEngine->Execute();
			m_RenderContext->GetAPI().Present();

			m_RenderContext->GetWindow().OnUpdate();

			b = true;
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
