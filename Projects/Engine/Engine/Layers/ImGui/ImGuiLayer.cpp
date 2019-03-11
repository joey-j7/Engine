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

static ImGui_ImplVulkanH_WindowData m_WindowData;
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
		ImGui_ImplVulkanH_WindowData* wd = &m_WindowData;

		Color clearColor(0.2f, 0.2f, 0.2f, 1.0f);
		memcpy(&wd->ClearValue.color.float32[0], &clearColor, 4 * sizeof(float));

		glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow* window, int width, int height)
		{
			RenderContext& context = Application::Get().GetRenderContext();
			RenderContextData& contextData = context.GetData();

			ImGui_ImplVulkanH_CreateWindowDataSwapChainAndFramebuffer(
				contextData.PhysicalDevice,
				contextData.Device,
				&m_WindowData,
				contextData.Allocator,
				width,
				height
			);
		});

		// SetupVulkanWindowData
		m_WindowData.Surface = contextData.Surface;

		// Check for WSI support
		VkBool32 res;

		vkGetPhysicalDeviceSurfaceSupportKHR(contextData.PhysicalDevice, contextData.QueueFamily, m_WindowData.Surface, &res);
		if (res != VK_TRUE)
		{
			fprintf(stderr, "Error no WSI support on physical device 0\n");
			exit(-1);
		}

		// Select Surface Format
		const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
		const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		m_WindowData.SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(contextData.PhysicalDevice, m_WindowData.Surface, requestSurfaceImageFormat, (sizeof(requestSurfaceImageFormat) / sizeof(*requestSurfaceImageFormat)), requestSurfaceColorSpace);

		// Select Present Mode (vsync on)
		VkPresentModeKHR present_modes[1];
		present_modes[0] = VK_PRESENT_MODE_FIFO_KHR;

		// VkPresentModeKHR present_modes[] = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_IMMEDIATE_KHR, VK_PRESENT_MODE_FIFO_KHR };

		m_WindowData.PresentMode = ImGui_ImplVulkanH_SelectPresentMode(contextData.PhysicalDevice, m_WindowData.Surface, &present_modes[0], (sizeof(present_modes) / sizeof(*present_modes)));
		//printf("[vulkan] Selected PresentMode = %d\n", m_WindowData.PresentMode);

		// Create SwapChain, RenderPass, Framebuffer, etc.
		ImGui_ImplVulkanH_CreateWindowDataCommandBuffers(contextData.PhysicalDevice, contextData.Device, contextData.QueueFamily, wd, contextData.Allocator);
		ImGui_ImplVulkanH_CreateWindowDataSwapChainAndFramebuffer(contextData.PhysicalDevice, contextData.Device, wd, contextData.Allocator, window.GetWidth(), window.GetHeight());

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
		init_info.CheckVkResultFn = [](VkResult err) { Application::Get().GetRenderContext().GetRenderer().Verify(err); };
		ImGui_ImplVulkan_Init(&init_info, m_WindowData.RenderPass);

		// Upload Fonts
		{
			// Use any command queue
			VkCommandPool command_pool = m_WindowData.Frames[m_WindowData.FrameIndex].CommandPool;
			VkCommandBuffer command_buffer = m_WindowData.Frames[m_WindowData.FrameIndex].CommandBuffer;

			int err = vkResetCommandPool(contextData.Device, command_pool, 0);
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
			err = vkQueueSubmit(contextData.Queue, 1, &end_info, VK_NULL_HANDLE);
			renderer.Verify(err);

			err = vkDeviceWaitIdle(contextData.Device);
			renderer.Verify(err);
			ImGui_ImplVulkan_InvalidateFontUploadObjects();
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
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();

		ImGui::DestroyContext();

		ImGui_ImplVulkanH_WindowData* wd = &m_WindowData;

		RenderContextData& contextData = Application::Get().GetRenderContext().GetData();
		ImGui_ImplVulkanH_DestroyWindowData(contextData.Instance, contextData.Device, wd, contextData.Allocator);
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

	void ImGuiLayer::OnImGuiRender()
	{
		static bool show = true;
		ImGui::ShowDemoWindow(&show);

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
		VkResult err;

		VkSemaphore& image_acquired_semaphore = m_WindowData.Frames[m_WindowData.FrameIndex].ImageAcquiredSemaphore;
		err = vkAcquireNextImageKHR(contextData.Device, m_WindowData.Swapchain, UINT64_MAX, image_acquired_semaphore, VK_NULL_HANDLE, &m_WindowData.FrameIndex);
		renderer.Verify(err);

		ImGui_ImplVulkanH_FrameData* fd = &m_WindowData.Frames[m_WindowData.FrameIndex];
		{
			err = vkWaitForFences(contextData.Device, 1, &fd->Fence, VK_TRUE, UINT64_MAX);	// wait indefinitely instead of periodically checking
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
			info.renderPass = m_WindowData.RenderPass;
			info.framebuffer = m_WindowData.Framebuffer[m_WindowData.FrameIndex];
			info.renderArea.extent.width = m_WindowData.Width;
			info.renderArea.extent.height = m_WindowData.Height;
			info.clearValueCount = 1;
			info.pClearValues = &m_WindowData.ClearValue;
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
			info.pSignalSemaphores = &fd->RenderCompleteSemaphore;

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
		VkPresentInfoKHR info = {};
		info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &fd->RenderCompleteSemaphore;
		info.swapchainCount = 1;
		info.pSwapchains = &m_WindowData.Swapchain;
		info.pImageIndices = &m_WindowData.FrameIndex;
		err = vkQueuePresentKHR(contextData.Queue, &info);
		renderer.Verify(err);
#endif
	}

}