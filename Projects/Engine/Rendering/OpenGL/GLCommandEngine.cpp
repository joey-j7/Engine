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

		// TODO
	}

	void GLCommandEngine::Reset()
	{
		if (m_State == E_RECORDING || m_State == E_IDLE)
			return;

		WaitForGPU();

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
		m_State = E_IDLE;
	}

	//void GLCommandEngine::Signal()
	//{
	//	// Increases fence value
	//	CommandEngine::Signal();

	//	if (m_uiFenceValue + 1 > m_Fences.size())
	//	{
	//		VkFenceCreateInfo fenceInfo = {};
	//		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	//		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	//		m_Fences.emplace_back();
	//		VkResult err = vkCreateFence(m_pContextData->Device, &fenceInfo, nullptr, &m_Fences.back());
	//		VkRenderAPI::Verify(err);
	//	}
	//}

	void GLCommandEngine::AdvanceFrame()
	{
		// If the next frame is not ready to be rendered yet, wait until it is ready.
		WaitForGPU();

		// Set the fence value for the next frame.
		m_uiFenceValue = 0;
	}
}