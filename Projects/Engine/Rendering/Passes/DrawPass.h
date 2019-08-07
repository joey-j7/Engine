#pragma once

#include "RenderPass.h"
#include "Engine/General/Math.h"

namespace Engine
{
	class Engine_API DrawPass : public RenderPass
	{
	public:
		DrawPass(const ShaderProgram::Descriptor& shaderPipeline);
		virtual ~DrawPass() = default;

		void Create(const ShaderProgram::Descriptor& shaderPipeline);

		virtual bool Clear();
		virtual bool Draw();

	protected:
		/* Native functions */
		uint32_t m_uiVertexCount = 3;
		uint32_t m_uiInstanceCount = 1;

		Vector3 m_ClearColor = Vector3(0.f, 0.f, 1.f);
	};
}
