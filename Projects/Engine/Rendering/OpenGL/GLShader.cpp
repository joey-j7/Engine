#include "pch.h"

#if CB_RENDERING_API == CB_RENDERER_OPENGL

#include "GLShader.h"

#include "Rendering/Resources/ShaderResource.h"

#include "Rendering/RenderDevice.h"
#include "Rendering/Shader.h"

#include "glad/glad.h"

#include "Engine/Application.h"
#include "Rendering/RenderContext.h"

namespace Engine
{
	GLShader::GLShader(const Shader::Descriptor& descriptor) : Shader(descriptor)
	{
#define LOAD_SHADER(x) m_Resources.push_back(static_cast<ShaderResource*>(Application::Get().GetRenderContext().GetRenderer().Load(x)))

		m_Handle = glCreateProgram();

		if (!descriptor.Compute.empty())
			LOAD_SHADER(descriptor.Compute);
		if (!descriptor.Pixel.empty())
			LOAD_SHADER(descriptor.Pixel);
		if (!descriptor.Geometry.empty())
			LOAD_SHADER(descriptor.Geometry);
		if (!descriptor.TessControl.empty())
			LOAD_SHADER(descriptor.TessControl);
		if (!descriptor.TessEvaluation.empty())
			LOAD_SHADER(descriptor.TessEvaluation);
		if (!descriptor.Vertex.empty())
			LOAD_SHADER(descriptor.Vertex);

		for (ShaderResource* resource : m_Resources)
		{
			glAttachShader(m_Handle, resource->GetHandle());
		}

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

		for (ShaderResource* resource : m_Resources)
		{
			Application::Get().GetRenderContext().GetRenderer().Unload(resource);
		}
	}
}

#endif