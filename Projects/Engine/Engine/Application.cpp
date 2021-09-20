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

	Application::Application(const String& sName) : LayeredObject(sName)
	{
		CB_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		
		m_Database = std::make_unique<FileDatabase>();

		m_RenderContext = std::make_shared<RenderContext>();
		m_RenderContext->Init();

		m_HardwareContext = std::make_shared<HardwareContext>();

		FileLoader::Init();

		m_pScreenEngine = m_RenderContext->GetAPI().GetCommandEngine("Screen");

		m_WorldManagerLayer = new WorldManagerLayer();
		PushOverlay(m_WorldManagerLayer);

		m_DeltaTime = std::make_unique<DeltaTime>();
		
		m_bInitialized = true;
	}

	Application::~Application()
	{
		// s_Instance = nullptr;
	}

	void Application::Run()
	{
		while (m_bRunning)
		{
			if (m_bPaused)
			{
				// Wait until new events occur (for example maximize)
				m_RenderContext->GetWindow().Wait();
				continue;
			}

			// One-time callback originating from other threads
			ThreadedCallback();
			ThreadedCallback.Clear();
			
			if (!m_RenderContext->GetAPI().Swap())
			{
				m_RenderContext->GetWindow().OnUpdate();
				continue;
			}

			// Retrieve delta time for logic
			m_DeltaTime->Update();
			const float fDeltaTime = m_DeltaTime->Get();

			OnUpdate(fDeltaTime);
			
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
			
			// Reset rendering for new frame
			m_RenderContext->GetAPI().Present();
			m_RenderContext->GetWindow().OnUpdate();
		}
	}

	void Application::SetPaused(bool Paused)
	{
		if (m_bPaused == Paused)
			return;
		
		m_bPaused = Paused;

		if (m_bPaused)
		{
			m_HardwareContext->Pause();

#ifdef CB_PLATFORM_ANDROID
			m_RenderContext->GetAPI().Deinit();
#endif
		}
		else
		{
			m_DeltaTime->Reset();

#ifdef CB_PLATFORM_ANDROID
			m_RenderContext->Deinit();
			m_RenderContext->Init();
#endif

			m_HardwareContext->Resume();
		}

		OnPauseChanged(m_bPaused);
	}

	void Application::OnWindowMinimize(const bool Minimized)
	{
		SetPaused(Minimized);
	}

	void Application::OnWindowFocus(const bool Focussed)
	{
		SetPaused(!Focussed);
	}

	void Application::OnWindowResize(uint32_t Width, uint32_t Height)
	{

	}

	void Application::OnWindowClose()
	{
		m_bRunning = false;
	}

	void Application::OnFramebufferResize(uint32_t Width, uint32_t Height)
	{
		GetRenderContext().GetAPI().OnFramebufferResize(Width, Height);
	}
}
