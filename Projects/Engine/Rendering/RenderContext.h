#pragma once

#include "Platform/Window.h"
#include "Rendering/Renderer.h"

#include "RenderContextData.h"

namespace Engine {
	class RenderContext
	{
	public:
		void Init();

		Window& GetWindow() const { return *m_Window; };
		Renderer& GetRenderer() const { return *m_Renderer; };
		RenderContextData& GetData() const { return *m_ContextData; };

	private:
		std::unique_ptr<Window> m_Window;
		std::unique_ptr<Renderer> m_Renderer;

		std::shared_ptr<RenderContextData> m_ContextData;

		bool m_Initialized = false;
	};
}