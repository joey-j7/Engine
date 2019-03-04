#include "pch.h"

#include "GLFWWindow.h"

#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/KeyEvent.h"

namespace Engine {

	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		CB_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
	}

	Window* Window::Create(const std::shared_ptr<RenderContextData>& contextData, const WindowProps& props)
	{
		return new GLFWWindow(contextData, props);
	}

	GLFWWindow::GLFWWindow(const std::shared_ptr<RenderContextData>& contextData, const WindowProps& props) : Window(contextData)
	{
		Init(props);
	}

	GLFWWindow::~GLFWWindow()
	{
		Shutdown();
	}

	void GLFWWindow::Init(const WindowProps& props)
	{
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		CB_CORE_INFO_T(5.0f, "Creating window {0} ({1} x {2})", props.Title, props.Width, props.Height);

		if (s_GLFWInitialized)
			Shutdown();

		if (!s_GLFWInitialized)
		{
			// TODO: glfwTerminate on system shutdown
			int success = glfwInit();
			CB_CORE_ASSERT(success, "Could not intialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);
			s_GLFWInitialized = true;
		}

#if CB_RENDERING_API == CB_RENDERER_VULKAN
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#else
#ifdef CB_PLATFORM_WINDOWS
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
#else
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#endif
		glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

		m_Window = glfwCreateWindow((int)m_Data.Width, (int)m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);

#if CB_RENDERING_API == CB_RENDERER_OPENGL
		glfwMakeContextCurrent(m_Window);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		CB_CORE_ASSERT(status, "Failed to initialize Glad!");
#endif

		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

#ifdef CB_PLATFORM_ANDROID
		// Resize to screen resolution
		int width, height;
		glfwGetWindowSize(m_Window, &width, &height);
		glfwSetWindowSize(m_Window, width, height);

		m_Data.Width = width;
		m_Data.Height = height;

#if CB_RENDERING_API == CB_RENDERER_OPENGL
		glViewport(0, 0, (GLsizei)width, (GLsizei)height);
		glScissor(0, 0, (GLsizei)width, (GLsizei)height);
#endif
#endif

		// Set GLFW callbacks
		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
		{
			Window::Data& data = *(Window::Data*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowIconifyCallback(m_Window, [](GLFWwindow* window, int iconified)
		{
			Window::Data& data = *(Window::Data*)glfwGetWindowUserPointer(window);
			WindowMinimizeEvent event(iconified);
			data.EventCallback(event);
		});

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focussed)
		{
			Window::Data& data = *(Window::Data*)glfwGetWindowUserPointer(window);
			WindowFocusEvent event(focussed);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
		{
			Window::Data& data = *(Window::Data*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			Window::Data& data = *(Window::Data*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				KeyPressedEvent event(key, 0);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				KeyReleasedEvent event(key);
				data.EventCallback(event);
				break;
			}
			case GLFW_REPEAT:
			{
				KeyPressedEvent event(key, 1);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
		{
			Window::Data& data = *(Window::Data*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			Window::Data& data = *(Window::Data*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				MouseButtonPressedEvent event(button);
				data.EventCallback(event);
				break;
			}
			case GLFW_RELEASE:
			{
				MouseButtonReleasedEvent event(button);
				data.EventCallback(event);
				break;
			}
			}
		});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			Window::Data& data = *(Window::Data*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
		{
			Window::Data& data = *(Window::Data*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	void GLFWWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
	}

	void GLFWWindow::OnUpdate()
	{
		glfwPollEvents();

#if CB_RENDERING_API == CB_RENDERER_OPENGL
		glfwSwapBuffers(m_Window);
#endif
	}

	void GLFWWindow::SetVSync(bool enabled)
	{
#if CB_RENDERING_API == CB_RENDERER_OPENGL
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
#endif

		m_Data.VSync = enabled;
	}

	bool GLFWWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	void GLFWWindow::Reset()
	{
		Init(WindowProps(m_Data.Title, m_Data.Width, m_Data.Height));
	}

}