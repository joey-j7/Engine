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
		OnMinimize.Bind(&Application::Get(), &Application::OnWindowMinimize);
		OnFocus.Bind(&Application::Get(), &Application::OnWindowFocus);
		OnClose.Bind(&Application::Get(), &Application::OnWindowClose);

		OnResize.Bind(&Application::Get(), &Application::OnWindowResize);
		OnFramebufferResize.Bind(&Application::Get(), &Application::OnFramebufferResize);
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
		glfwSetWindowSizeCallback(m_Window, OnResizeCallback);
		glfwSetFramebufferSizeCallback(m_Window, OnFramebufferResizeCallback);
		
		glfwSetWindowIconifyCallback(m_Window, OnMinimizeCallback);
		glfwSetWindowFocusCallback(m_Window, OnFocusCallback);
		glfwSetWindowCloseCallback(m_Window, OnCloseCallback);

		glfwSetKeyCallback(m_Window, OnKeyCallback);
		glfwSetCharCallback(m_Window, OnCharCallback);

		glfwSetMouseButtonCallback(m_Window, OnMouseButtonCallback);
		glfwSetScrollCallback(m_Window, OnScrollCallback);
		glfwSetCursorPosCallback(m_Window, OnCursorPositionCallback);

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

	bool GLFWWindow::IsMousePressed() const
	{
		return glfwGetMouseButton(m_Window, 0) == GLFW_PRESS;
	}

	DVector2 GLFWWindow::GetMousePosition() const
	{
		DVector2 Position;
		glfwGetCursorPos(m_Window, &Position.x, &Position.y);

		return Position;
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

	void GLFWWindow::GetSize(int32_t& Width, int32_t& Height) const
	{
		glfwGetFramebufferSize(m_Window, &Width, &Height);
	}

	void GLFWWindow::OnResizeCallback(GLFWwindow* Window, int Width, int Height)
	{
		// Identified as a minimize event, not necessary
		if (Width <= 0 || Height <= 0)
			return;

		GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);

		AppWindow.m_Data.Width = Width;
		AppWindow.m_Data.Height = Height;

		AppWindow.OnResize(Width, Height);
	}

	void GLFWWindow::OnFramebufferResizeCallback(GLFWwindow* Window, int Width, int Height)
	{
		// Identified as a minimize event, not necessary
		if (Width <= 0 || Height <= 0)
			return;

		GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);

		AppWindow.m_Data.Width = Width;
		AppWindow.m_Data.Height = Height;

		AppWindow.OnFramebufferResize(Width, Height);
	}

	void GLFWWindow::OnMinimizeCallback(GLFWwindow* Window, int Minimized)
	{
		GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
		AppWindow.OnMinimize(Minimized == 1);
	}

	void GLFWWindow::OnFocusCallback(GLFWwindow* Window, int Focussed)
	{
		GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
		AppWindow.OnFocus(Focussed == 1);
	}

	void GLFWWindow::OnCloseCallback(GLFWwindow* Window)
	{
		GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
		AppWindow.OnClose();
	}

	void GLFWWindow::OnKeyCallback(GLFWwindow* Window, int Key, int Scancode, int Action, int Mods)
	{
		switch (Action)
		{
		case GLFW_PRESS:
		{
			GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
			AppWindow.OnKeyPressed(Key, 0);
			break;
		}
		case GLFW_RELEASE:
		{
			GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
			AppWindow.OnKeyReleased(Key);
			break;
		}
		case GLFW_REPEAT:
		{
			GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
			AppWindow.OnKeyPressed(Key, 1);
			break;
		}
		}
	}

	void GLFWWindow::OnCharCallback(GLFWwindow* Window, unsigned Keycode)
	{
		GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
		AppWindow.OnChar(Keycode);
	}

	void GLFWWindow::OnMouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Mods)
	{
		switch (Action)
		{
		case GLFW_PRESS:
		{
			GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
			AppWindow.OnMousePressed(Button);
			break;
		}
		case GLFW_RELEASE:
		{
			GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
			AppWindow.OnMouseReleased(Button);
			break;
		}
		}
	}

	void GLFWWindow::OnScrollCallback(GLFWwindow* Window, double OffsetX, double OffsetY)
	{
		GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
		AppWindow.OnScroll(DVector2(OffsetX, OffsetY));
	}
	
	void GLFWWindow::OnCursorPositionCallback(GLFWwindow* Window, double PositionX, double PositionY)
	{
		GLFWWindow& AppWindow = *(GLFWWindow*)glfwGetWindowUserPointer(Window);
		AppWindow.OnCursorPosition(DVector2(PositionX, PositionY));
	}
}
