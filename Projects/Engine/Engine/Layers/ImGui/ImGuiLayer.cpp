#include "pch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "Rendering/RenderContext.h"

#if CB_RENDERING_API == CB_RENDERER_OPENGL
#include "examples/imgui_impl_opengl3.h"
#include "examples/imgui_impl_glfw.h"
#include <glad/glad.h>
#elif CB_RENDERING_API == CB_RENDERER_VULKAN
//#include "examples/imgui_impl_vulkan.h"
//#include "examples/imgui_impl_glfw.h"

//#include "Rendering/Vulkan/VkRenderAPI.h"
//#include "Rendering/Vulkan/VkCommandEngine.h"

static int                      m_iMinImageCount = 2;
#endif

#include "Engine/Application.h"

namespace Engine
{
	VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
	VkPipelineCache PipelineCache = VK_NULL_HANDLE;

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach(const LayerStack& stack)
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
		RenderContext& renderContext = app.GetRenderContext();
		Window& window = renderContext.GetWindow();
		GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window.GetNativeWindow());

		// Setup Platform/RenderAPI bindings
#if CB_RENDERING_API == CB_RENDERER_OPENGL
		ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
#ifdef CB_PLATFORM_WINDOWS
		ImGui_ImplOpenGL3_Init("#version 410");
#else 
		ImGui_ImplOpenGL3_Init("#version 300 es");
#endif
#elif CB_RENDERING_API == CB_RENDERER_VULKAN
		// Create Descriptor Pool
		{
			VkDescriptorPoolSize pool_sizes[] =
			{
				{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
				{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
				{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
				{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
			};

			VkDescriptorPoolCreateInfo pool_info = {};
			pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
			pool_info.maxSets = 1000 * ((int)(sizeof(pool_sizes) / sizeof(*pool_sizes)));
			pool_info.poolSizeCount = (uint32_t)((int)(sizeof(pool_sizes) / sizeof(*pool_sizes)));
			pool_info.pPoolSizes = pool_sizes;
			//VkResult err = vkCreateDescriptorPool(contextData.Device, &pool_info, contextData.Allocator, &DescriptorPool);
			//VkRenderAPI::Verify(err);
		}

		// Setup Platform/RenderAPI bindings
		//ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);
		//ImGui_ImplVulkan_InitInfo init_info = {};
		//init_info.Instance = contextData.Instance;
		//init_info.PhysicalDevice = contextData.PhysicalDevice;
		//init_info.Device = contextData.Device;
		//init_info.QueueFamily = contextData.QueueFamily;
		//init_info.Queue = contextData.Queue;
		//init_info.PipelineCache = PipelineCache;
		//init_info.DescriptorPool = DescriptorPool;
		//init_info.Allocator = contextData.Allocator;
		//init_info.MinImageCount = contextData.SwapchainCtx.MinImageCount;
		//init_info.ImageCount = contextData.SwapchainCtx.Views.size();
		//init_info.CheckVkResultFn = [](VkResult err) { VkRenderAPI::Verify(err); };
		//ImGui_ImplVulkan_Init(&init_info, m_Window.RenderPass);

		// Upload Fonts
		{
			//RenderContextData& renderData = renderContext.GetData();
			//RenderAPI& api = renderContext.GetAPI();
			//VkCommandEngine* pEngine = static_cast<VkCommandEngine*>(api.GetCommandEngine("Screen"));

			//// Use any command queue
			//VkCommandPool command_pool = pEngine->GetCommandPool();
			//VkCommandBuffer command_buffer = pEngine->GetCommandBuffer();

			//VkResult err = vkResetCommandPool(renderData.Device, command_pool, 0);
			//VkRenderAPI::Verify(err);
			//VkCommandBufferBeginInfo begin_info = {};
			//begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			//begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			//err = vkBeginCommandBuffer(command_buffer, &begin_info);
			//VkRenderAPI::Verify(err);

			//ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

			//VkSubmitInfo end_info = {};
			//end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			//end_info.commandBufferCount = 1;
			//end_info.pCommandBuffers = &command_buffer;
			//err = vkEndCommandBuffer(command_buffer);
			//VkRenderAPI::Verify(err);
			//err = vkQueueSubmit(renderData.Queue, 1, &end_info, VK_NULL_HANDLE);
			//VkRenderAPI::Verify(err);

			//err = vkDeviceWaitIdle(renderData.Device);
			//VkRenderAPI::Verify(err);
			//ImGui_ImplVulkan_DestroyFontUploadObjects();
		}
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

	bool ImGuiLayer::OnDetach(const LayerStack& stack)
	{
		if (!OnDetach(stack))
			return false;

#if CB_RENDERING_API == CB_RENDERER_OPENGL
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImGui::DestroyContext();
#elif CB_RENDERING_API == CB_RENDERER_VULKAN

		Application& app = Application::Get();
		/*RenderContext& renderContext = app.GetRenderContext();
		RenderContextData& renderData = renderContext.GetData();

		VkResult err = vkDeviceWaitIdle(renderData.Device);
		VkRenderAPI::Verify(err);

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImGui::DestroyContext();

		RenderContextData& contextData = Application::Get().GetRenderContext().GetData();
		vkDestroyDescriptorPool(contextData.Device, DescriptorPool, contextData.Allocator);*/

#ifdef CB_DEBUG
		// Remove the debug report callback
		//auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(contextData.Instance, "vkDestroyDebugReportCallbackEXT");
		//vkDestroyDebugReportCallbackEXT(contextData.Instance, contextData.DebugReport, contextData.Allocator);
#endif // IMGUI_VULKAN_DEBUG_REPORT
#endif

		return true;
	}


	void ImGuiLayer::Begin()
	{
		if (!IsActive())
			return;

#if CB_RENDERING_API == CB_RENDERER_OPENGL
		ImGui_ImplOpenGL3_NewFrame();
#elif CB_RENDERING_API == CB_RENDERER_VULKAN
		//ImGui_ImplVulkan_NewFrame();
#endif

		//ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		if (!IsActive())
			return;

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		RenderContext& renderContext = app.GetRenderContext();
		Window& window = renderContext.GetWindow();

		io.DisplaySize = ImVec2((float)window.GetWidth(), (float)window.GetHeight());

		// Rendering
		ImGui::Render();

#if CB_RENDERING_API == CB_RENDERER_OPENGL
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
#elif CB_RENDERING_API == CB_RENDERER_VULKAN
		RenderAPI& api = renderContext.GetAPI();
		// VkCommandEngine* pCmdEngine = static_cast<VkCommandEngine*>(api.GetCommandEngine("Screen"));
		
		// FrameRender
		// pCmdEngine->Reset();
		// pCmdEngine->Start();

		/*{
			VkRenderPassBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.renderPass = m_RenderPass;
			info.framebuffer = fd->Framebuffer;
			info.renderArea.extent.width = window.GetWidth();
			info.renderArea.extent.height = window.GetHeight();
			info.clearValueCount = 1;
			info.pClearValues = &m_Window.ClearValue;
			vkCmdBeginRenderPass(pCmdEngine->GetCommandBuffer(), &info, VK_SUBPASS_CONTENTS_INLINE);
		}*/

		// Record Imgui Draw Data and draw funcs into command buffer
		//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), pCmdEngine->GetCommandBuffer());

		// End render pass
		//vkCmdEndRenderPass(pCmdEngine->GetCommandBuffer());

		// Submit command buffer
		// pCmdEngine->Execute();

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		api.Present();
#endif
	}

	void ImGuiLayer::Draw(float fDeltaTime)
	{
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		static bool show = true;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &show, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}

		if (ImGui::BeginMenuBar())
		{;
			ImGui::Text(("  " + std::to_string(DeltaTime::GetFPS())).c_str(), NULL, false);

			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit", NULL, false))
					Application::Get().Exit();

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();

		// On-Screen Logger
#ifndef CB_DIST
		auto& logger = ::Engine::Log::GetScreenLogger();

		if (!logger.empty())
		{
			auto& io = ImGui::GetIO();

			ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, 0.0f), ImVec2(io.DisplaySize.x - 10.0f, io.DisplaySize.y - 10.0f));
			ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowPos(ImVec2(5.0f, 25.0f));
			ImGui::SetNextWindowBgAlpha(0.8f);
			// ImGui::SetNextWindowFocus();

			ImGui::Begin("Logger", &show, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoInputs);
			ImGui::PushTextWrapPos(io.DisplaySize.x - 10.0f);

			for (uint32_t i = 0; i < logger.size(); ++i)
			{
				ScreenMessage& log = logger[i];
				const char* message = log.Message.c_str();

				ImGui::TextColored(
					ImVec4(log.Color.r, log.Color.g, log.Color.b, log.Color.z),
					message
				);

				if (log.Time.Run())
				{
					logger.erase(logger.begin() + i);
				}
			}

			ImGui::PopTextWrapPos();
			ImGui::End();
		}
#endif
	}
}