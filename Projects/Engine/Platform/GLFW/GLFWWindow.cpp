#include "pch.h"

#include "GLFWWindow.h"

#include "Engine/Events/ApplicationEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/KeyEvent.h"

#include "Rendering/RenderContext.h"

#include <glad/glad.h>

namespace Engine {

	static bool s_GLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		CB_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
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
		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		if (s_GLFWInitialized)
			Shutdown();

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

		glfwSetWindowUserPointer(m_Window, &m_Data);
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

		CB_CORE_INFO("Created window {0} with dimensions ({1} x {2})", props.Title, props.Width, props.Height);
	}

	void GLFWWindow::Shutdown()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void GLFWWindow::OnUpdate()
	{
		glfwPollEvents();

		if (RenderContext::GetAPIType() == RenderAPI::E_OPENGL)
		{
			glfwSwapBuffers(m_Window);
		}
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
		Init(WindowProps(m_Data.Title, m_Data.Width, m_Data.Height));
	}

}
