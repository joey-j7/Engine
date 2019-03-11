#include "pch.h"

#if CB_RENDERING_API == CB_RENDERER_OPENGL

#include "GLShader.h"

#include "Rendering/Resources/ShaderResource.h"

#include "Rendering/RenderDevice.h"
#include "Rendering/Shader.h"

#include "glad/glad.h"

namespace Engine
{
	GLShader::GLShader(const Shader::Descriptor& descriptor) : Shader(descriptor)
	{
		m_Handle = glCreateProgram();

		if (descriptor.Compute)
			glAttachShader(m_Handle, descriptor.Compute->GetHandle());
		if (descriptor.Pixel)
			glAttachShader(m_Handle, descriptor.Pixel->GetHandle());
		if (descriptor.Geometry)
			glAttachShader(m_Handle, descriptor.Geometry->GetHandle());
		if (descriptor.TessControl)
			glAttachShader(m_Handle, descriptor.TessControl->GetHandle());
		if (descriptor.TessEvaluation)
			glAttachShader(m_Handle, descriptor.TessEvaluation->GetHandle());
		if (descriptor.Vertex)
			glAttachShader(m_Handle, descriptor.Vertex->GetHandle());

		glLinkProgram(m_Handle);

		GLint isLinked = 0;
		glGetProgramiv(m_Handle, GL_LINK_STATUS, &isLinked);
		m_Linked = isLinked;

		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_Handle, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);

			if (maxLength > 0)
				glGetProgramInfoLog(m_Handle, maxLength, &maxLength, &infoLog[0]);

			CB_CORE_WARN("Could not create shader program, output:\n", infoLog.data());
			return;
		}

		CB_CORE_INFO("Succesfully created shader program {0}", m_Handle);
	}

	GLShader::~GLShader()
	{
		glDeleteProgram(m_Handle);
		CB_CORE_INFO("Succesfully deleted shader program {0}", m_Handle);
	}
}

#endif