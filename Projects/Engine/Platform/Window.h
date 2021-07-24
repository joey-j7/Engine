#pragma once

#include <string>

#include "Engine/General/Common.h"

#include "Engine/Core.h"
#include "Engine/Events/Event.h"

#include "Engine/General/Math.h"

namespace Engine {

	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		WindowProps(
			const std::string& title = TOSTRING(CB_NAME),
			uint32_t width = 1280,
			uint32_t height = 720
		)
			: Title(title), Width(width), Height(height)
		{
		}
	};

	// Interface representing a desktop system based Window
	class Engine_API Window
	{
		friend class Application;

	public:
		struct Data
		{
			std::string Title;
			unsigned int Width, Height;
			float Scale;

			bool VSync = true;
			bool TrippleBuffering = false;
		};

		Window();
		virtual ~Window() {}

		virtual void OnUpdate() = 0;
		
		virtual void GetSize(int32_t& Width, int32_t& Height) const = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual float GetScale() const = 0;

		virtual void* GetWindow() { return nullptr; }

		// Window attributes
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void SetTrippleBuffering(bool enabled) = 0;
		virtual bool IsTrippleBuffering() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());
		virtual void Reset() = 0;

		virtual bool IsMousePressed() const = 0;
		virtual DVector2 GetMousePosition() const = 0;

		virtual void Wait() = 0;

		Event<void, uint32_t, uint32_t> OnResize = Event<void, uint32_t, uint32_t>("Window::OnResize");
		Event<void, uint32_t, uint32_t> OnFramebufferResize = Event<void, uint32_t, uint32_t>("Window::OnFramebufferResize");
		
		Event<void, bool> OnMinimize = Event<void, bool>("Window::OnMinimize");
		Event<void, bool> OnFocus = Event<void, bool>("Window::OnFocus");
		Event<void> OnClose = Event<void>("Window::OnClose");

		Event<void, uint32_t> OnChar = Event<void, uint32_t>("Window::OnChar");

		Event<void, uint32_t> OnMousePressed = Event<void, uint32_t>("Window::OnMousePressed");
		Event<void, uint32_t> OnMouseReleased = Event<void, uint32_t>("Window::OnMouseReleased");

		Event<void, int32_t, int32_t> OnKeyPressed = Event<void, int32_t, int32_t>("Window::OnKeyPressed");
		Event<void, int32_t> OnKeyReleased = Event<void, int32_t>("Window::OnKeyReleased");
		
		Event<void, double, double> OnScroll = Event<void, double, double>("Window::OnScroll");
		Event<void, double, double> OnCursorPosition = Event<void, double, double>("Window::OnCursorPosition");

	protected:
		virtual void Shutdown() = 0;

		Data m_Data;
	};

}