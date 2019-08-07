#pragma once

#include "Rendering/ShaderProgram.h"
#include "Engine/Core.h"

#include <memory>

namespace Engine
{
	class Engine_API RenderPass
	{
	public:
		RenderPass(const ShaderProgram::Descriptor& shaderPipeline);
		virtual ~RenderPass() {}

		/* Used for barriers, for example */
		virtual bool Begin();
		virtual bool End();

	protected:
		std::unique_ptr<ShaderProgram> m_pShaderProgram = nullptr;
		bool m_bBegun = false;
	};
}
