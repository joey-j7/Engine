#include "pch.h"

#include "GLFWWindow.h"
#include "Rendering/RenderContext.h"

#include <glad/glad.h>

#include "Engine/Application.h"

namespace Engine {

	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		CB_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window::Window()
	{
		OnMinimizeEvent.Bind(&Application::Get(), &Application::OnWindowMinimize);
		OnFocusEvent.Bind(&Application::Get(), &Application::OnWindowFocus);
		OnResizeEvent.Bind(&Application::Get(), &Application::OnWindowResize);
		OnCloseEvent.Bind(&Application::Get(), &Application::OnWindowClose);
	}

	Window* Window::Create(const WindowProps& props)
	{
		return new GLFWWindow(props);
	}

	GLFWWindow::GLFWWindow(const WindowProps& props) : Window()
	{
		Init(props);
	}

	GLFWWindow::~GLFWWindow()
	{
		Shutdown();
	}

	void GLFWWindow::Init(const WindowProps& props)
	{
		/* Don't recreate */
		if (m_Window)
			return;
		
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		if (!s_GLFWInitialized)
		{
			int success = glfwInit(); (void)success;
			CB_CORE_ASSERT(success, "Could not intialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

		if (RenderContext::GetAPIType() == RenderAPI::E_VULKAN)
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}
		else
		{
#ifdef CB_PLATFORM_WINDOWS
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
#else
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
			glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif
		}

		m_Window = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);

		if (RenderContext::GetAPIType() == RenderAPI::E_OPENGL)
		{
			glfwMakeContextCurrent(m_Window);

			int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
			CB_CORE_ASSERT(status, "Failed to initialize Glad!");
		}

		glfwSetWindowUserPointer(m_Window, this);
		SetVSync(true);
		SetTrippleBuffering(false);

		glfwGetWindowContentScale(
			m_Window,
			&m_Data.Scale,
			&m_Data.Scale
		);

#ifdef CB_PLATFORM_ANDROID
		// Resize to screen resolution
		int width, height;
		glfwGetWindowSize(m_Window, &width, &height);
		glfwSetWindowSize(m_Window, width, height);

		m_Data.Width = width;
		m_Data.Height = height;

		if (RenderContext::GetAPIType() == RenderAPI::E_OPENGL)
		{
			glViewport(0, 0, (GLsizei)width, (GLsizei)height);
			glScissor(0, 0, (GLsizei)width, (GLsizei)height);
		}

		m_Data.Scale /= 160.f;
#endif

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* Window, int Width, int Height) { OnResize(Window, Width, Height); });
		glfwSetWindowIconifyCallback(m_Window, OnMinimize);
		glfwSetWindowFocusCallback(m_Window, OnFocus);
		glfwSetWindowCloseCallback(m_Window, OnClose);

		glfwSetKeyCallback(m_Window, OnKey);
		glfwSetCharCallback(m_Window, OnChar);

		glfwSetMouseButtonCallback(m_Window, OnMouseButton);
		glfwSetScrollCallback(m_Window, OnScroll);
		glfwSetCursorPosCallback(m_Window, OnCursorPosition);

		CB_CORE_INFO("Created a window {0} with dimensions ({1} x {2})", props.Title, props.Width, props.Height);
	}

	void GLFWWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void GLFWWindow::SetVSync(bool enabled)
	{
		if (RenderContext::GetAPIType() == RenderAPI::E_OPENGL)
		{
			if (enabled)
				glfwSwapInterval(1);
			else
				glfwSwapInterval(0);
		}

		m_Data.VSync = enabled;
	}

	bool GLFWWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	void GLFWWindow::SetTrippleBuffering(bool enabled)
	{
		m_Data.TrippleBuffering = enabled;
	}

	bool GLFWWindow::IsTrippleBuffering() const
	{
		return m_Data.TrippleBuffering;
	}

	void GLFWWindow::Reset()
	{
		glfwDestroyWindow(m_Window);
		m_Window = nullptr;

		const WindowProps props(m_Data.Title, m_Data.Width, m_Data.Height);
		Init(props);
	}

	void GLFWWindow::Wait()
	{
		glfwWaitEvents();
	}

	/* Events */
	void GLFWWindow::OnUpdate()
	{
		glfwPollEvents();

		if (RenderContext::GetAPIType() == RenderAPI::E_OPENGL)
		{
			glfwSwapBuffers(m_Window);
		}
	}
	
	void GLFWWindow::OnResize(GLFWwindow* Window, int Width, int Height)
	{
		// Identified as a minimize event, not necessary
		if (Width == 0 || Height == 0)
			return;

		GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);

		AppWindow.m_Data.Width = Width;
		AppWindow.m_Data.Height = Height;

		AppWindow.OnResizeEvent(Width, Height);
	}
	
	void GLFWWindow::OnMinimize(GLFWwindow* Window, int Minimized)
	{
		GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
		AppWindow.OnMinimizeEvent(Minimized == 1);
	}

	void GLFWWindow::OnFocus(GLFWwindow* Window, int Focussed)
	{
		GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
		AppWindow.OnFocusEvent(Focussed == 1);
	}

	void GLFWWindow::OnClose(GLFWwindow* Window)
	{
		GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
		AppWindow.OnCloseEvent();
	}

	void GLFWWindow::OnKey(GLFWwindow* Window, int Key, int Scancode, int Action, int Mods)
	{
		switch (Action)
		{
		case GLFW_PRESS:
		{
			GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
			AppWindow.OnKeyPressedEvent(Key, 0);
			break;
		}
		case GLFW_RELEASE:
		{
			GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
			AppWindow.OnKeyReleasedEvent(Key);
			break;
		}
		case GLFW_REPEAT:
		{
			GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
			AppWindow.OnKeyPressedEvent(Key, 1);
			break;
		}
		}
	}

	void GLFWWindow::OnChar(GLFWwindow* Window, unsigned Keycode)
	{
		GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
		AppWindow.OnCharEvent(Keycode);
	}

	void GLFWWindow::OnMouseButton(GLFWwindow* Window, int Button, int Action, int Mods)
	{
		switch (Action)
		{
		case GLFW_PRESS:
		{
			GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
			AppWindow.OnMousePressedEvent(Button);
			break;
		}
		case GLFW_RELEASE:
		{
			GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
			AppWindow.OnMouseReleasedEvent(Button);
			break;
		}
		}
	}

	void GLFWWindow::OnScroll(GLFWwindow* Window, double xOffset, double yOffset)
	{
		GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
		AppWindow.OnScrollEvent(xOffset, yOffset);
	}
	
	void GLFWWindow::OnCursorPosition(GLFWwindow* Window, double xPos, double yPos)
	{
		GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
		AppWindow.OnCursorPositionEvent(xPos, yPos);
	}
}
