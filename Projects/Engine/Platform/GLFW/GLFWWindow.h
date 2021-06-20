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

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		inline unsigned int GetHeight() const override { return m_Data.Height; }

		inline float GetScale() const override { return m_Data.Scale; };
		
		inline void* GetWindow() override
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

		static void OnResize(GLFWwindow* window, int width, int height);
		static void OnMinimize(GLFWwindow* window, int iconified);
		static void OnFocus(GLFWwindow* window, int focussed);
		static void OnClose(GLFWwindow* window);
		
		static void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void OnChar(GLFWwindow* window, unsigned int keycode);
		
		static void OnMouseButton(GLFWwindow* window, int button, int action, int mods);
		static void OnScroll(GLFWwindow* window, double xOffset, double yOffset);
		static void OnCursorPosition(GLFWwindow* window, double xPos, double yPos);

		GLFWwindow* m_Window = nullptr;
	};
}
