#pragma once

#include "Platform/Window.h"
#include "Rendering/RenderAPI.h"

namespace Engine
{
	class Engine_API RenderContext
	{
	public:
		~RenderContext();

		void Init();
		void Deinit();

		Window& GetWindow() const { return *m_pWindow; };
		
		// Upload
		void Upload(const String& m_FilePath);
		RenderAPI& GetAPI() const { return *m_pAPI; };

		static RenderAPI::Type GetAPIType() { return m_APIType; }
		void SetAPIType(RenderAPI::Type type);

		bool IsInitialized() const { return m_Initialized; }

	private:
		std::unique_ptr<Window> m_pWindow;
		std::unique_ptr<RenderAPI> m_pAPI;

		static RenderAPI::Type m_APIType;

		bool m_Initialized = false;
	};
}
