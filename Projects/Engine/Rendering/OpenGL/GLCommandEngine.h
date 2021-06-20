#pragma once

#include "Rendering/CommandEngine.h"
#include <vulkan/vulkan.h>

#include "Engine/Logger/Log.h"

struct RenderContextData;

namespace Engine
{
	class Engine_API GLCommandEngine : public CommandEngine
	{
	public:
		GLCommandEngine(Type type, const std::string& sName = "Command Engine");
		virtual ~GLCommandEngine();

		virtual DrawPass* Create(const ShaderProgram::Descriptor& shaderPipeline) override;

		// Reset allocator
		virtual void Reset() override;

		// Reset command List
		virtual void Begin() override;

		// Close command list and execute
		virtual void Execute() override;

		// Apply all pending barriers
		virtual void ApplyBarriers() override
		{
			CB_CORE_ASSERT(false, "Not implemented");
		};

		// Wait until GPU has finished executing
		virtual void WaitForGPU() override;

	protected:
		// virtual void Signal() override;
	};
}
