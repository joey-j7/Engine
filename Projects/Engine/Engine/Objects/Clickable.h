#pragma once

#include "Platform/Window.h"

namespace Engine
{
	class Engine_API Clickable
	{
	public:
		Clickable();
		virtual ~Clickable();

		virtual const AABB GetBounds() const = 0;
		
	protected:
		virtual void OnEnter(double XPosition, double YPosition);
		virtual void OnHover(double XPosition, double YPosition);
		virtual void OnExit(double XPosition, double YPosition);

		virtual void OnPressed(double XPosition, double YPosition);
		virtual void OnReleased(double XPosition, double YPosition);

		virtual void OnClicked(double XPosition, double YPosition);

		bool m_IsHovered = false;
		bool m_IsPressed = false;

	private:
		void OnCursorPosition(double XPosition, double YPosition);

		Window& m_Window;
	};
}
