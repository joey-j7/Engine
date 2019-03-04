#pragma once

#include "Core.h"

#include "Window.h"
#include "Renderer.h"

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

		inline Window& GetWindow() { return *m_Window; }

		inline static Application& Get() { return *s_Instance; }

	private:
		bool OnAppPause(AppPauseEvent& e);

		bool OnWindowMinimize(WindowMinimizeEvent& e);
		bool OnWindowFocus(WindowFocusEvent& e);
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_Window;
		std::unique_ptr<Renderer> m_Renderer;

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