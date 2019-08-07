#pragma once

#include "Rendering/ShaderProgram.h"
#include "../Files/GLShaderFile.h"

#include <vector>

namespace Engine
{
	class GLShaderProgram : public ShaderProgram
	{
	public:
		GLShaderProgram(const ShaderProgram::Descriptor& descriptor);
		virtual ~GLShaderProgram();

		virtual void Bind() override;
		virtual void Unbind() override;

	private:
		uint32_t m_uiHandle = UINT_MAX;
		std::vector<GLShaderFile*> m_Resources;
	};
}
