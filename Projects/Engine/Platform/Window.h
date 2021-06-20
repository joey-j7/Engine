#pragma once

#include <string>

#include "Engine/General/Common.h"

#include "Engine/Core.h"
#include "Engine/Events/Event.h"

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

		virtual void Wait() = 0;

		Event<void, uint32_t, uint32_t> OnResizeEvent;
		Event<void, bool> OnMinimizeEvent;
		Event<void, bool> OnFocusEvent;
		Event<void> OnCloseEvent;

		Event<void, uint32_t> OnCharEvent;

		Event<void, uint32_t> OnMousePressedEvent;
		Event<void, uint32_t> OnMouseReleasedEvent;

		Event<void, int32_t, int32_t> OnKeyPressedEvent;
		Event<void, int32_t> OnKeyReleasedEvent;
		
		Event<void, double, double> OnScrollEvent;
		Event<void, double, double> OnCursorPositionEvent;

	protected:
		virtual void Shutdown() = 0;

		Data m_Data;
	};

}