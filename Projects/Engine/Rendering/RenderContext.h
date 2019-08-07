#pragma once

#include "Platform/Window.h"
#include "Rendering/RenderAPI.h"

namespace Engine
{
	class Engine_API RenderContext
	{
	public:
		void Init();

		// Upload
		void Upload(const std::string& m_FilePath);
		RenderAPI& GetAPI() const { return *m_pAPI; };

		Window& GetWindow() const { return *m_pWindow; };

		static RenderAPI::Type GetAPIType() { return m_APIType; }
		void SetAPIType(RenderAPI::Type type);

	private:
		std::unique_ptr<RenderAPI> m_pAPI;
		std::unique_ptr<Window> m_pWindow;

		static RenderAPI::Type m_APIType;

		bool m_Initialized = false;
	};
}
