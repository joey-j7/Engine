#include "pch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "Rendering/RenderContext.h"

#if CB_RENDERING_API == CB_RENDERER_OPENGL
#include "examples/imgui_impl_opengl3.h"
#include "examples/imgui_impl_glfw.h"
#include <glad/glad.h>
#elif CB_RENDERING_API == CB_RENDERER_VULKAN
#include "examples/imgui_impl_vulkan.h"
#include "examples/imgui_impl_glfw.h"

static ImGui_ImplVulkanH_Window m_Window;
static int                      m_iMinImageCount = 2;
#endif

#include "Engine/Application.h"

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
		RenderContext& renderContext = app.GetRenderContext();
		Window& window = renderContext.GetWindow();
		RenderContextData& contextData = renderContext.GetData();
		GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window.GetNativeWindow());

		// Setup Platform/Renderer bindings
#if CB_RENDERING_API == CB_RENDERER_OPENGL
		ImGui_ImplGlfw_InitForOpenGL(glfwWindow, true);
#ifdef CB_PLATFORM_WINDOWS
		ImGui_ImplOpenGL3_Init("#version 410");
#else 
		ImGui_ImplOpenGL3_Init("#version 300 es");
#endif
#elif CB_RENDERING_API == CB_RENDERER_VULKAN
		// Create Framebuffers
		ImGui_ImplVulkanH_Window* wd = &m_Window;

		Color clearColor(0.2f, 0.2f, 0.2f, 1.0f);
		memcpy(&wd->ClearValue.color.float32[0], &clearColor, 4 * sizeof(float));

		glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* window, int w, int h)
		{	
			RenderContext& context = Application::Get().GetRenderContext();
			RenderContextData& contextData = context.GetData();

			ImGui_ImplVulkan_SetMinImageCount(m_iMinImageCount);
			ImGui_ImplVulkanH_CreateWindow(contextData.Instance, contextData.PhysicalDevice, contextData.Device, &m_Window, contextData.QueueFamily, contextData.Allocator, w, h, m_iMinImageCount);
			m_Window.FrameIndex = 0;
		});

		// SetupVulkanWindowData
		m_Window.Surface = contextData.Surface;

		// Check for WSI support
		VkBool32 res;

		vkGetPhysicalDeviceSurfaceSupportKHR(contextData.PhysicalDevice, contextData.QueueFamily, m_Window.Surface, &res);
		if (res != VK_TRUE)
		{
			fprintf(stderr, "Error no WSI support on physical device 0\n");
			exit(-1);
		}

		// Select Surface Format
		const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
		const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		m_Window.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(contextData.PhysicalDevice, m_Window.Surface, requestSurfaceImageFormat, (sizeof(requestSurfaceImageFormat) / sizeof(*requestSurfaceImageFormat)), requestSurfaceColorSpace);

		// Select Present Mode (vsync on)
		VkPresentModeKHR present_modes[1];
		present_modes[0] = VK_PRESENT_MODE_FIFO_KHR;

		// VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };

		m_Window.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(contextData.PhysicalDevice, m_Window.Surface, &present_modes[0], (sizeof(present_modes) / sizeof(*present_modes)));
		//printf("[vulkan] Selected PresentMode = %d\n", m_Window.PresentMode);

		// Create SwapChain, RenderPass, Framebuffer, etc.
		ImGui_ImplVulkanH_CreateWindow(contextData.Instance, contextData.PhysicalDevice, contextData.Device, &m_Window, contextData.QueueFamily, contextData.Allocator, window.GetWidth(), window.GetHeight(), m_iMinImageCount);

		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance = contextData.Instance;
		init_info.PhysicalDevice = contextData.PhysicalDevice;
		init_info.Device = contextData.Device;
		init_info.QueueFamily = contextData.QueueFamily;
		init_info.Queue = contextData.Queue;
		init_info.PipelineCache = contextData.PipelineCache;
		init_info.DescriptorPool = contextData.DescriptorPool;
		init_info.Allocator = contextData.Allocator;
		init_info.MinImageCount = m_iMinImageCount;
		init_info.ImageCount = m_Window.ImageCount;
		init_info.CheckVkResultFn = [](VkResult err) { Application::Get().GetRenderContext().GetRenderer().Verify(err); };
		ImGui_ImplVulkan_Init(&init_info, m_Window.RenderPass);

		// Upload Fonts
		{
			Renderer& renderer = renderContext.GetRenderer();
			RenderContextData& renderData = renderContext.GetData();

			// Use any command queue
			VkCommandPool command_pool = wd->Frames[wd->FrameIndex].CommandPool;
			VkCommandBuffer command_buffer = wd->Frames[wd->FrameIndex].CommandBuffer;

			VkResult err = vkResetCommandPool(renderData.Device, command_pool, 0);
			renderer.Verify(err);
			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			err = vkBeginCommandBuffer(command_buffer, &begin_info);
			renderer.Verify(err);

			ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

			VkSubmitInfo end_info = {};
			end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			end_info.commandBufferCount = 1;
			end_info.pCommandBuffers = &command_buffer;
			err = vkEndCommandBuffer(command_buffer);
			renderer.Verify(err);
			err = vkQueueSubmit(renderData.Queue, 1, &end_info, VK_NULL_HANDLE);
			renderer.Verify(err);

			err = vkDeviceWaitIdle(renderData.Device);
			renderer.Verify(err);
			ImGui_ImplVulkan_DestroyFontUploadObjects();
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

	void ImGuiLayer::OnDetach()
	{
#if CB_RENDERING_API == CB_RENDERER_OPENGL
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImGui::DestroyContext();
#elif CB_RENDERING_API == CB_RENDERER_VULKAN

		Application& app = Application::Get();
		RenderContext& renderContext = app.GetRenderContext();
		RenderContextData& renderData = renderContext.GetData();
		Renderer& renderer = renderContext.GetRenderer();

		VkResult err = vkDeviceWaitIdle(renderData.Device);
		renderer.Verify(err);
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImGui::DestroyContext();

		RenderContextData& contextData = Application::Get().GetRenderContext().GetData();
		ImGui_ImplVulkanH_DestroyWindow(contextData.Instance, contextData.Device, &m_Window, contextData.Allocator);
		vkDestroyDescriptorPool(contextData.Device, contextData.DescriptorPool, contextData.Allocator);

#ifdef CB_DEBUG
		// Remove the debug report callback
		auto vkDestroyDebugReportCallbackEXT = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(contextData.Instance, "vkDestroyDebugReportCallbackEXT");
		vkDestroyDebugReportCallbackEXT(contextData.Instance, contextData.DebugReport, contextData.Allocator);
#endif // IMGUI_VULKAN_DEBUG_REPORT

		vkDestroyDevice(contextData.Device, contextData.Allocator);
		vkDestroyInstance(contextData.Instance, contextData.Allocator);
#endif
	}


	void ImGuiLayer::Begin()
	{
#if CB_RENDERING_API == CB_RENDERER_OPENGL
		ImGui_ImplOpenGL3_NewFrame();
#elif CB_RENDERING_API == CB_RENDERER_VULKAN
		ImGui_ImplVulkan_NewFrame();
#endif

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
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
		Renderer& renderer = renderContext.GetRenderer();
		RenderContextData& contextData = renderContext.GetData();
		
		// FrameRender
		VkResult err;

		VkSemaphore image_acquired_semaphore = m_Window.FrameSemaphores[m_Window.SemaphoreIndex].ImageAcquiredSemaphore;
		VkSemaphore render_complete_semaphore = m_Window.FrameSemaphores[m_Window.SemaphoreIndex].RenderCompleteSemaphore;
		err = vkAcquireNextImageKHR(contextData.Device, m_Window.Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &m_Window.FrameIndex);
		renderer.Verify(err);

		ImGui_ImplVulkanH_Frame* fd = &m_Window.Frames[m_Window.FrameIndex];
		{
			err = vkWaitForFences(contextData.Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);    // wait indefinitely instead of periodically checking
			renderer.Verify(err);

			err = vkResetFences(contextData.Device, 1, &fd->Fence);
			renderer.Verify(err);
		}
		{
			err = vkResetCommandPool(contextData.Device, fd->CommandPool, 0);
			renderer.Verify(err);
			VkCommandBufferBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			err = vkBeginCommandBuffer(fd->CommandBuffer, &info);
			renderer.Verify(err);
		}
		{
			VkRenderPassBeginInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.renderPass = m_Window.RenderPass;
			info.framebuffer = fd->Framebuffer;
			info.renderArea.extent.width = m_Window.Width;
			info.renderArea.extent.height = m_Window.Height;
			info.clearValueCount = 1;
			info.pClearValues = &m_Window.ClearValue;
			vkCmdBeginRenderPass(fd->CommandBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
		}

		// Record Imgui Draw Data and draw funcs into command buffer
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), fd->CommandBuffer);

		// Submit command buffer
		vkCmdEndRenderPass(fd->CommandBuffer);
		{
			VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			VkSubmitInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			info.waitSemaphoreCount = 1;
			info.pWaitSemaphores = &image_acquired_semaphore;
			info.pWaitDstStageMask = &wait_stage;
			info.commandBufferCount = 1;
			info.pCommandBuffers = &fd->CommandBuffer;
			info.signalSemaphoreCount = 1;
			info.pSignalSemaphores = &render_complete_semaphore;

			err = vkEndCommandBuffer(fd->CommandBuffer);
			renderer.Verify(err);
			err = vkQueueSubmit(contextData.Queue, 1, &info, fd->Fence);
			renderer.Verify(err);
		}

		// Update and Render additional Platform Windows
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}

		// FramePresent
		render_complete_semaphore = m_Window.FrameSemaphores[m_Window.SemaphoreIndex].RenderCompleteSemaphore;
		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &render_complete_semaphore;
		info.swapchainCount = 1;
		info.pSwapchains = &m_Window.Swapchain;
		info.pImageIndices = &m_Window.FrameIndex;

		err = vkQueuePresentKHR(contextData.Queue, &info);
		renderer.Verify(err);
		m_Window.SemaphoreIndex = (m_Window.SemaphoreIndex + 1) % m_Window.ImageCount; // Now we can use the next set of semaphores
#endif
	}

	void ImGuiLayer::OnImGuiRender()
	{
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

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
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::MenuItem("Exit", NULL, true);
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::End();

		ImGui::Begin("Style Editor");
		ImGui::ShowStyleEditor();
		ImGui::End();

		// On-Screen Logger
#ifndef CB_DIST
		auto& logger = ::Engine::Log::GetScreenLogger();

		if (!logger.empty())
		{
			auto& io = ImGui::GetIO();

			ImGui::SetNextWindowSizeConstraints(ImVec2(0.0f, 0.0f), ImVec2(io.DisplaySize.x - 10.0f, io.DisplaySize.y - 10.0f));
			ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowPos(ImVec2(5.0f, 5.0f));
			ImGui::SetNextWindowBgAlpha(0.8f);
			ImGui::SetNextWindowFocus();

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