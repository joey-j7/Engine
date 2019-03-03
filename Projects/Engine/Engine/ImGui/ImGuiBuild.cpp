#include "pch.h"

#if CB_RENDERING_API == CB_RENDERER_OPENGL
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "examples/imgui_impl_opengl3.cpp"
#elif CB_RENDERING_API == CB_RENDERER_VULKAN
#include "examples/imgui_impl_vulkan.cpp"
#endif

#include "examples/imgui_impl_glfw.cpp"