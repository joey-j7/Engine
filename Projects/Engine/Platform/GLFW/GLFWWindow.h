#pragma once

#include "pch.h"
#include "Platform/Window.h"

namespace Engine {

	class GLFWWindow : public Window
	{
	public:
		GLFWWindow(const std::shared_ptr<RenderContextData>& contextData, const WindowProps& props);
		virtual ~GLFWWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		inline void* GetWindow() override
		{
			return m_Window;
		}

		// Window attributes
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const override { return m_Window; }
		virtual void Reset() override;

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown() override;

	private:
		GLFWwindow* m_Window = nullptr;
	};

}