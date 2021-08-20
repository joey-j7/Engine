#pragma once

#include "pch.h"
#include "Platform/Window.h"

namespace Engine {

	class Engine_API GLFWWindow : public Window
	{
	public:
		GLFWWindow(const WindowProps& props);
		virtual ~GLFWWindow();

		void OnUpdate() override;

		void GetSize(int32_t& Width, int32_t& Height) const override;
		
		unsigned int GetWidth() const override { return m_Data.Width; }
		unsigned int GetHeight() const override { return m_Data.Height; }

		float GetScale() const override { return m_Data.Scale; };
		
		void* GetWindow() override
		{
			return m_Window;
		}

		// Window attributes
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;
		
		void SetTrippleBuffering(bool enabled) override;
		bool IsTrippleBuffering() const override;

		inline virtual void* GetNativeWindow() const override { return m_Window; }
		virtual void Reset() override;

		virtual void Wait() override;
		
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown() override;

		static void OnResizeCallback(GLFWwindow* window, int width, int height);
		static void OnFramebufferResizeCallback(GLFWwindow* window, int width, int height);
		
		static void OnMinimizeCallback(GLFWwindow* window, int iconified);
		static void OnFocusCallback(GLFWwindow* window, int focussed);
		static void OnCloseCallback(GLFWwindow* window);
		
		static void OnKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void OnCharCallback(GLFWwindow* window, unsigned int keycode);
		
		static void OnMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void OnScrollCallback(GLFWwindow* window, double OffsetX, double OffsetY);
		static void OnCursorPositionCallback(GLFWwindow* window, double PositionX, double PositionY);

		GLFWwindow* m_Window = nullptr;
	};
}
