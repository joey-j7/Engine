#include "pch.h"
#include "Window.h"

namespace Engine
{
	bool Window::m_MousePressed = false;
	bool Window::m_MouseInView = false;
	DVector2 Window::m_MousePosition = DVector2(0.f);
	DVector2 Window::m_MouseDelta = DVector2(0.f);
	Window::Data Window::m_Data;
}