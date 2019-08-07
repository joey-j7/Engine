#include "pch.h"

#include "GLShaderProgram.h"

#include "../Files/GLShaderFile.h"
#include "Rendering/ShaderProgram.h"

#include "../GLRenderAPI.h"
#include "glad/glad.h"

#include "Rendering/RenderContext.h"

namespace Engine
{
	GLShaderProgram::GLShaderProgram(const ShaderProgram::Descriptor& descriptor) : ShaderProgram(descriptor)
	{
#define LOAD_SHADER(x) m_Resources.push_back(static_cast<GLShaderFile*>(GLRenderAPI::Get().Load(x)))

		m_uiHandle = glCreateProgram();

		if (!descriptor.Compute.empty())
			LOAD_SHADER(descriptor.Compute + ".glsl");
		if (!descriptor.Pixel.empty())
			LOAD_SHADER(descriptor.Pixel + ".glsl");
		if (!descriptor.Geometry.empty())
			LOAD_SHADER(descriptor.Geometry + ".glsl");
		if (!descriptor.TessControl.empty())
			LOAD_SHADER(descriptor.TessControl + ".glsl");
		if (!descriptor.TessEvaluation.empty())
			LOAD_SHADER(descriptor.TessEvaluation + ".glsl");
		if (!descriptor.Vertex.empty())
			LOAD_SHADER(descriptor.Vertex + ".glsl");

		for (GLShaderFile* resource : m_Resources)
		{
			glAttachShader(m_uiHandle, resource->GetHandle());
		}

		glLinkProgram(m_uiHandle);

		GLint isLinked = 0;
		glGetProgramiv(m_uiHandle, GL_LINK_STATUS, &isLinked);
		m_bLinked = isLinked;

		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_uiHandle, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);

			if (maxLength > 0)
			{
				glGetProgramInfoLog(m_uiHandle, maxLength, &maxLength, &infoLog[0]);
				CB_CORE_WARN("Could not create shader program, output:\n", infoLog.data());
			}
			else
			{
				CB_CORE_WARN("Could not create shader program, no output result");
			}

			return;
		}

		CB_CORE_INFO("Succesfully created shader program {0}", m_uiHandle);
	}

	GLShaderProgram::~GLShaderProgram()
	{
		if (m_uiHandle != UINT_MAX)
		{
			glDeleteProgram(m_uiHandle);
			CB_CORE_INFO("Succesfully deleted shader program {0}", m_uiHandle);
		}

		auto& api = GLRenderAPI::Get();

		for (ShaderFile* resource : m_Resources)
		{
			api.Unload(resource);
		}
	}

	void GLShaderProgram::Bind()
	{
		if (m_uiHandle != UINT_MAX)
			glUseProgram(m_uiHandle);
	}

	void GLShaderProgram::Unbind()
	{
		glUseProgram(0);
	}
}