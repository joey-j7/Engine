#include "pch.h"

#include "GLDrawPass.h"

#include "../GLCommandEngine.h"
#include "Engine/Logger/Log.h"

#include <glad/glad.h>

namespace Engine
{
	GLDrawPass::GLDrawPass(GLCommandEngine& engine, const ShaderProgram::Descriptor& shaderPipeline) : DrawPass(shaderPipeline)
	{
		m_pCommandEngine = &engine;
	}

	GLDrawPass::~GLDrawPass()
	{
	}

	bool GLDrawPass::Begin()
	{
		if (m_pCommandEngine->GetState() != CommandEngine::E_RECORDING)
		{
			CB_CORE_WARN("Tried to begin a draw pass while the command engine is busy or hasn't been started yet!");
			return false;
		}

		if (!DrawPass::Begin())
			return false;

		glCullFace(GL_NONE);

		return true;
	}

	bool GLDrawPass::Draw()
	{
		if (!DrawPass::Draw())
			return false;

		glClearColor(m_ClearColor.r, m_ClearColor.g, m_ClearColor.b, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		if (m_uiVertexCount * m_uiInstanceCount == 0)
		{
			CB_CORE_WARN("The current draw pass has nothing to draw!");
			return false;
		}

		if (m_uiInstanceCount == 1)
			glDrawArrays(GL_TRIANGLES, 0, m_uiVertexCount);
		else
			glDrawArraysInstanced(GL_TRIANGLES, 0, m_uiVertexCount, m_uiInstanceCount);

		return true;
	}

	bool GLDrawPass::End()
	{
		if (!DrawPass::End())
			return false;

		return true;
	}
}
