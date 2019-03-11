#pragma once

#include "Rendering/Shader.h"

namespace Engine
{
	class GLShader : public Shader
	{
	public:
		GLShader(const Shader::Descriptor& descriptor);
		virtual ~GLShader();
	};
}