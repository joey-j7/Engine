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
		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
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

	private:
		GLFWwindow* m_Window = nullptr;
	};

}