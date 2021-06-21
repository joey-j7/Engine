#pragma once

#include "../Engine/Objects/Object.h"
#include "ShaderProgram.h"

#include <vector>
#include "Rendering/Passes/DrawPass.h"

namespace Engine
{
	class Engine_API CommandEngine : public Object
	{
	public:
		enum Type
		{
			E_DIRECT,
			E_COPY,
			E_COMPUTE
		};

		enum State
		{
			E_CREATED,
			E_IDLE,
			E_RECORDING,
			E_EXECUTING
		};

		CommandEngine(Type type, const std::string& sName = "Command Engine") : Object(sName)
		{
			m_Type = type;
		}

		virtual ~CommandEngine() = default;

		virtual DrawPass* Create(const ShaderProgram::Descriptor& shaderPipeline) = 0;

		// Reset allocator
		virtual void Reset() = 0;

		// Reset command List
		virtual void Begin() = 0;

		// Close command list and execute
		virtual void Execute() = 0;

		// Apply all pending barriers
		virtual void ApplyBarriers() = 0;

		// Wait until GPU has finished executing
		virtual void WaitForGPU() = 0;

		// Get engine type
		Type GetType() const { return m_Type; }

		// Check if engine has started
		State GetState() const { return m_State; }

		// Get current fence value
		uint64_t GetValue() const { return m_uiFenceValue; }

		// Get draw passes
		const std::vector<DrawPass*>& GetPasses() const { return m_Passes; };

	protected:
		// Signal a value to fence
		virtual void Signal()
		{
			m_uiFenceValue++;
		}

		Type m_Type = E_DIRECT;
		State m_State = E_CREATED;

		std::vector<DrawPass*> m_Passes;

		uint64_t m_uiFenceValue = 0;
	};
}
