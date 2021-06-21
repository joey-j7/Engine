#pragma once

#include "Core.h"

#include "Rendering/RenderContext.h"
#include "Platform/Hardware/HardwareContext.h"

#include "Engine/Layers/World/WorldManagerLayer.h"

#include "Objects/LayeredObject.h"

namespace Engine
{
	class Engine_API Application : public LayeredObject
	{
		friend class Window;

	public:
		Application(const std::string& sName = "Application");
		virtual ~Application();

		void Run();

		// TODO: Move to platform context
		HardwareContext& GetHardwareContext() const { return *m_HardwareContext; }
		RenderContext& GetRenderContext() const { return *m_RenderContext; }
		
		FileDatabase& GetFileDatabase() const { return *m_Database; }
		
		static Application& Get() { return *s_Instance; }

		WorldManagerLayer& GetWorldManager() const { return *m_WorldManagerLayer; }

		void Exit() { m_bRunning = false; }

		Event<void, bool> OnPauseChangedEvent;

	private:
		void SetPaused(bool Paused);

		void OnWindowMinimize(const bool Minimized);
		void OnWindowFocus(const bool Focussed);
		void OnWindowResize(uint32_t Width, uint32_t Height);
		void OnWindowClose();

		void OnFramebufferResize(uint32_t Width, uint32_t Height);

		std::shared_ptr<RenderContext> m_RenderContext;
		std::shared_ptr<HardwareContext> m_HardwareContext;
		
		CommandEngine* m_pScreenEngine;

		std::unique_ptr<FileDatabase> m_Database = nullptr;

		WorldManagerLayer* m_WorldManagerLayer = nullptr;

		std::unique_ptr<DeltaTime> m_DeltaTime;

		bool m_bInitialized = false;
		bool m_bRunning = true;
		bool m_bPaused = false;

		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();

}
