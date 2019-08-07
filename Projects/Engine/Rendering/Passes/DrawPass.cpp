#include "pch.h"
#include "DrawPass.h"

namespace Engine
{
	DrawPass::DrawPass(const ShaderProgram::Descriptor& shaderPipeline) : RenderPass(shaderPipeline)
	{

	}

	bool DrawPass::Clear()
	{
		if (!m_bBegun)
		{
			CB_CORE_WARN("Tried to clear a draw pass while it hasn't been started yet!");
			return false;
		}

		return true;
	}

	bool DrawPass::Draw()
	{
		if (!m_bBegun)
		{
			CB_CORE_WARN("Tried to draw a draw pass while it hasn't been started yet!");
			return false;
		}

		m_pShaderProgram->Bind();
		return true;
	}

}
