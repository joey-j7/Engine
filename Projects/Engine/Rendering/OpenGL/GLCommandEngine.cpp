#include "pch.h"

#include "GLCommandEngine.h"
#include "GLRenderAPI.h"

#include "Passes/GLDrawPass.h"

namespace Engine
{
	DrawPass* GLCommandEngine::Create(const ShaderProgram::Descriptor& shaderPipeline)
	{
		return new GLDrawPass(*this, shaderPipeline);
	}

	GLCommandEngine::GLCommandEngine(Type type, const std::string& sName) : CommandEngine(type, sName)
	{
	}

	GLCommandEngine::~GLCommandEngine()
	{
		// Ensure that the GPU is no longer referencing resources that are about to be
		// cleaned up by the destructor.
		WaitForGPU();
		m_State = E_IDLE;

		// TODO
	}

	void GLCommandEngine::Reset()
	{
		if (m_State == E_RECORDING || m_State == E_IDLE)
			return;

		WaitForGPU();
		m_State = E_IDLE;

		// TODO
	}

	void GLCommandEngine::Begin()
	{
		if (m_State != E_IDLE) return;
		m_State = E_RECORDING;

		// TODO
	}

	void GLCommandEngine::Execute()
	{
		if (m_State != E_RECORDING) return;
		m_State = E_EXECUTING;

		// TODO

		// Signal
		Signal();
	}

	void GLCommandEngine::WaitForGPU()
	{
		// TODO
	}

	//void GLCommandEngine::Signal()
	//{
	//	// Increases fence value
	//	CommandEngine::Signal();

	//	if (m_uiFenceValue + 1 > m_UsageFences.size())
	//	{
	//		VkFenceCreateInfo fenceInfo = {};
	//		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	//		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	//		m_UsageFences.emplace_back();
	//		VkResult err = vkCreateFence(m_pContextData->Device, &fenceInfo, nullptr, &m_UsageFences.back());
	//		VkRenderAPI::Verify(err);
	//	}
	//}
}