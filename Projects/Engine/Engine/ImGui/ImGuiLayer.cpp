#include "pch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"
#include "examples/imgui_impl_glfw.h"

#include "Engine/Application.h"

// TEMPORARY
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Engine {

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.IniFilename = NULL;

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		// Setup Dear ImGui Style
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowBorderSize = 0.0f;

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);

#ifdef CB_PLATFORM_WINDOWS
		ImGui_ImplOpenGL3_Init("#version 410");
#else 
		ImGui_ImplOpenGL3_Init("#version 300 es");
#endif

#ifdef CB_PLATFORM_ANDROID
		float scale = 3.0f;
		io.FontGlobalScale *= scale;
		style.ScaleAllSizes(scale);

		style.FramePadding = ImVec2(25.f, 25.f);
		style.TouchExtraPadding = ImVec2(25.f, 25.f);
		style.ScrollbarSize = 0.f;

		io.ConfigFlags |= ImGuiConfigFlags_IsTouchScreen;

		io.ConfigWindowsResizeFromEdges = false;
		io.ConfigWindowsMoveFromTitleBarOnly = true;
#endif
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnImGuiRender()
	{
		static bool show = true;
		ImGui::ShowDemoWindow(&show);

#ifndef CB_DIST
		// Logger
		auto& logger = ::Engine::Log::GetScreenLogger();

		if (!logger.empty())
		{
			ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowPos(ImVec2(5.0f, 5.0f));
			ImGui::SetNextWindowBgAlpha(0.5f);

			ImGui::Begin("Logger", &show, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoInputs);

			for (uint32_t i = 0; i < logger.size(); ++i)
			{
				ImGui::TextColored(ImVec4(logger[i].Color.r, logger[i].Color.g, logger[i].Color.b, logger[i].Color.z), logger[i].Message.c_str());

				if (logger[i].Time.Run())
				{
					logger.erase(logger.begin() + i);
				}
			}

			ImGui::End();
		}
		// Logger
#endif
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

}