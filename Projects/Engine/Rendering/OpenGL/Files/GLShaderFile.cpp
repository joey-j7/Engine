#include "pch.h"

#include "GLShaderFile.h"
#include "Platform/FileLoader.h"

#include <string>

namespace Engine
{
	GLShaderFile::GLShaderFile(const std::string& filePath) : ShaderFile(filePath)
	{
		uint32_t length = 0;
		const char* data = FileLoader::Read(filePath, length, FileLoader::E_ROOT, false);

		// Convert type to native
		switch(m_Type)
		{
		case VERTEX:
			m_GLType = GL_VERTEX_SHADER;
			break;
		case PIXEL:
			m_GLType = GL_FRAGMENT_SHADER;
			break;
		case COMPUTE:
			m_GLType = GL_COMPUTE_SHADER;
			break;
		case GEOMETRY:
			m_GLType = GL_GEOMETRY_SHADER;
			break;
		case TESS_CONTROL:
			m_GLType = GL_TESS_CONTROL_SHADER;
			break;
		case TESS_EVALUATION:
			m_GLType = GL_TESS_EVALUATION_SHADER;
			break;
		default:
			// Unsupported
			CB_CORE_ASSERT(false, "Unsupported shader type detected!");
			break;
		}

		// Create an empty vertex shader handle
		m_uiHandle = glCreateShader(m_GLType);

		if (filePath.find(".glsl") != std::string::npos)
		{
			std::string s = ParseIncludes(filePath, data, length);
			s.insert(s.find_first_of('\n') + 1, "#define API_GLSL\r\n");

			const char* d = s.c_str();
			GLint l = static_cast<GLint>(s.length());

			glShaderSource(m_uiHandle, 1, &d, &l);

			// Compile the vertex shader
			glCompileShader(m_uiHandle);
		}
		else
		{
			glShaderBinary(1, &m_uiHandle, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, data, length);

			static const std::string entryPoint = "main"; // Get entry point name
			glSpecializeShader(m_uiHandle, (const GLchar*)entryPoint.c_str(), 0, nullptr, nullptr);
		}

		delete[] data;

		GLint isCompiled = 0;
		glGetShaderiv(m_uiHandle, GL_COMPILE_STATUS, &isCompiled);

		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(m_uiHandle, GL_INFO_LOG_LENGTH, &maxLength);

			if (maxLength > 0)
			{
				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(m_uiHandle, maxLength, &maxLength, &infoLog[0]);

				CB_CORE_ERROR("Failed to compile shader \"{0}\", output:\n{1}", filePath, infoLog.data());
			}
			else
			{
				CB_CORE_ERROR("Failed to compile shader \"{0}\", no output", filePath);
			}

			CB_CORE_ASSERT(false, "Error while compiling shader!");
			
			// We don't need the shader anymore.
			glDeleteShader(m_uiHandle);
			m_uiHandle = UINT_MAX;

			return;
		}
	}

	GLShaderFile::~GLShaderFile()
	{
		if (m_uiHandle != UINT_MAX)
			glDeleteShader(m_uiHandle);
	}
}
