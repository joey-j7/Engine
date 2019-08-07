#pragma once

#include "Rendering/Passes/DrawPass.h"

namespace Engine
{
	class GLCommandEngine;

	class Engine_API GLDrawPass : public DrawPass
	{
	public:
		GLDrawPass(GLCommandEngine& engine, const ShaderProgram::Descriptor& shaderPipeline);
		virtual ~GLDrawPass();

		virtual bool Begin() override;
		virtual bool Draw() override;
		virtual bool End() override;

	private:
		GLCommandEngine* m_pCommandEngine = nullptr;
	};
}
