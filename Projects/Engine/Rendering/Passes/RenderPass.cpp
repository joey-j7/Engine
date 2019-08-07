#include "pch.h"
#include "RenderPass.h"

#include "Engine/Application.h"

namespace Engine
{
	RenderPass::RenderPass(const ShaderProgram::Descriptor& shaderPipeline)
	{
		m_pShaderProgram = std::unique_ptr<ShaderProgram>(
			Application::Get().GetRenderContext().GetAPI().Create(shaderPipeline)
		);
	}

	bool RenderPass::Begin()
	{
		if (m_bBegun)
		{
			// Pass has already begun!
			CB_CORE_WARN("Tried to begin a render pass when it has already begin!");

			return false;
		}

		m_bBegun = true;
		return true;
	}

	bool RenderPass::End()
	{
		if (!m_bBegun)
		{
			// Pass has already ended!
			CB_CORE_WARN("Tried to end a render pass when it has already ended!");

			return false;
		}

		m_bBegun = false;
		return true;
	}
}
