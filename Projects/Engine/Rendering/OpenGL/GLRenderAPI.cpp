#include "pch.h"

#include "GLRenderAPI.h"
#include "Rendering/RenderAPI.h"

#include "Files/GLShaderFile.h"

namespace Engine 
{
	GLRenderAPI* GLRenderAPI::s_Instance = nullptr;

	GLRenderAPI::GLRenderAPI(RenderContext& renderContext) : RenderAPI(renderContext)
	{
		CB_CORE_ASSERT(!s_Instance, "VkRenderAPI already exists!");
		s_Instance = this;

		FileDatabase::Get().Add<GLShaderFile>({ "vspv", "glsl" });

		m_CommandEngines.emplace("Screen", new GLCommandEngine(CommandEngine::E_DIRECT, "Screen"));
		m_ScreenCommandEngine = m_CommandEngines["Screen"];
	}

	bool GLRenderAPI::Swap()
	{
		return true;
	}

	void GLRenderAPI::Present()
	{
		
	}

	CommandEngine* GLRenderAPI::GetCommandEngine(const String& sName)
	{
		auto it = m_CommandEngines.find(sName);

		if (it == m_CommandEngines.end())
			return nullptr;

		return it->second;
	}
}