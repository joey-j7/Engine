#pragma once

#include "Core.h"

#include "Rendering/RenderContext.h"

#include "Engine/LayerStack.h"
#include "Engine/Events/Event.h"
#include "Engine/Events/ApplicationEvent.h"

#include "Engine/ImGui/ImGuiLayer.h"

namespace Engine {

	class Engine_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline RenderContext& GetRenderContext() { return *m_RenderContext; }
		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnAppPause(AppPauseEvent& e);

		bool OnWindowMinimize(WindowMinimizeEvent& e);
		bool OnWindowFocus(WindowFocusEvent& e);
		bool OnWindowClose(WindowCloseEvent& e);

		std::shared_ptr<RenderContext> m_RenderContext;
		ImGuiLayer* m_ImGuiLayer;

		std::unique_ptr<DeltaTime> m_DeltaTime;

		bool m_Running = true;
		bool m_Paused = false;

		LayerStack m_LayerStack;

	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}