#pragma once

#include "Core.h"

#include "Rendering/RenderContext.h"
#include "Platform/Hardware/HardwareContext.h"

#include "Engine/Objects/Worlds/WorldManager.h"

#include "Engine/Objects/Layers/Layerable.h"

namespace Engine
{
	class Engine_API Application : public Object, Layerable<>
	{
		friend class Window;
		friend class World;

	public:
		Application(const String& sName = "Application");
		virtual ~Application();

		void Run();

		// TODO: Move to platform context
		HardwareContext& GetHardwareContext() const { return *m_HardwareContext; }
		RenderContext& GetRenderContext() const { return *m_RenderContext; }
		
		FileDatabase& GetFileDatabase() const { return *m_Database; }
		WorldManager& GetWorldManager() const { return *m_WorldManager; }

		static Application& Get() { return *s_Instance; }

		void Exit() { m_bRunning = false; }

		Event<void> ThreadedCallback = Event<void>("Application::ThreadedCallback");
		Event<void, bool> OnPauseChanged = Event<void, bool>("Application::OnPauseChanged");

	private:
		Event<void, float> OnUpdate = Event<void, float>("Application::OnUpdate");
		Event<void, float> OnFixedUpdate = Event<void, float>("Application::OnFixedUpdate");
		Event<void, float> OnLateUpdate = Event<void, float>("Application::OnLateUpdate");
		Event<void, float> OnDraw = Event<void, float>("Application::OnDraw");

		void SetPaused(bool Paused);

		void OnWindowMinimize(const bool Minimized);
		void OnWindowFocus(const bool Focussed);
		void OnWindowResize(uint32_t Width, uint32_t Height);
		void OnWindowClose();

		void OnFramebufferResize(uint32_t Width, uint32_t Height);

		std::shared_ptr<RenderContext> m_RenderContext;
		std::shared_ptr<HardwareContext> m_HardwareContext;
		
		CommandEngine* m_pScreenEngine = nullptr;

		std::unique_ptr<FileDatabase> m_Database = nullptr;

		// Managed by layerable
		WorldManager* m_WorldManager = nullptr;

		std::unique_ptr<DeltaTime> m_DeltaTime;

		bool m_bInitialized = false;
		bool m_bRunning = true;
		bool m_bPaused = false;

		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}
