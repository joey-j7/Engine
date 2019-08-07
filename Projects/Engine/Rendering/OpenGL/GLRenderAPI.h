#pragma once

#include "Rendering/RenderAPI.h"
#include "Objects/GLShaderProgram.h"

#include "GLCommandEngine.h"

namespace Engine {
	class Engine_API GLRenderAPI : public RenderAPI
	{
	public:
		GLRenderAPI(RenderContext& renderContext);

		static GLRenderAPI& Get() { return *s_Instance; }

		virtual ShaderProgram* Create(const ShaderProgram::Descriptor& descriptor) override { return new GLShaderProgram(descriptor); }

		virtual void Swap() override;
		virtual void Present() override;

		virtual CommandEngine* GetCommandEngine(const std::string& sName) override;

	private:
		static GLRenderAPI* s_Instance;

		std::unordered_map<std::string, GLCommandEngine*> m_CommandEngines;
		GLCommandEngine* m_ScreenCommandEngine = nullptr;
	};
}
