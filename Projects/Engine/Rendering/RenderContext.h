#pragma once

#include "Platform/Window.h"
#include "Rendering/Renderer.h"

#include "Rendering/RenderContextData.h"

namespace Engine
{
	class Engine_API RenderContext
	{
	public:
		void Init();

		// Upload
		void Upload(const std::string& m_FilePath);

		Window& GetWindow() const { return *m_pWindow; };
		Renderer& GetRenderer() const { return *m_pRenderer; };
		RenderContextData& GetData() const { return *m_pContextData; };

	protected:
		std::unique_ptr<Window> m_pWindow;
		std::unique_ptr<Renderer> m_pRenderer;
		std::shared_ptr<RenderContextData> m_pContextData;

	private:
		bool m_Initialized = false;
	};
}