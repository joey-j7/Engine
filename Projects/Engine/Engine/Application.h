#pragma once

#include "Core.h"

#include "Rendering/RenderContext.h"

#include "Engine/Events/Event.h"
#include "Engine/Events/ApplicationEvent.h"

#include "Engine/Layers/World/WorldManagerLayer.h"

#include "Objects/LayeredObject.h"

namespace Engine
{
	class Engine_API Application : public LayeredObject
	{
	public:
		Application(const std::string& sName = "Application");
		virtual ~Application();

		void Run();
		virtual void Call(Event& e) override;

		RenderContext& GetRenderContext() const { return *m_RenderContext; }
		static Application& Get() { return *s_Instance; }

		WorldManagerLayer& GetWorldManager() const { return *m_WorldManagerLayer; }

		void Exit() { m_bRunning = false; }

	private:
		bool OnAppPause(AppPauseEvent& e);

		bool OnWindowMinimize(WindowMinimizeEvent& e);
		bool OnWindowFocus(WindowFocusEvent& e);
		bool OnWindowClose(WindowCloseEvent& e);

		std::shared_ptr<RenderContext> m_RenderContext;
		CommandEngine* m_pScreenEngine;

		WorldManagerLayer* m_WorldManagerLayer = nullptr;

		std::unique_ptr<DeltaTime> m_DeltaTime;

		bool m_bRunning = true;
		bool m_bPaused = false;

		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}
