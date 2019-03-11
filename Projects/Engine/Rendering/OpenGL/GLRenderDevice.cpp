#include "pch.h"

#if CB_RENDERING_API == CB_RENDERER_OPENGL

#include "GLRenderDevice.h"
#include "glad/glad.h"

#include "Rendering/Renderer.h"

#include "Rendering/Resources/ModelResource.h"
#include "Rendering/Resources/TextureResource.h"
#include "Rendering/Resources/ShaderResource.h"

#include "Rendering/OpenGL/GLShader.h"
#include "Platform/FileLoader.h"

namespace Engine
{
	void GLRenderDevice::LoadModel(ModelResource* model)
	{
		CB_CORE_INFO("Attempted to load model");
	}

	void GLRenderDevice::UnloadModel(ModelResource* model)
	{
		CB_CORE_INFO("Attempted to unload model");
	}

	void GLRenderDevice::LoadTexture(TextureResource* texture)
	{
		CB_CORE_INFO("Attempted to load texture");
	}

	void GLRenderDevice::UnloadTexture(TextureResource* texture)
	{
		CB_CORE_INFO("Attempted to unload texture");
	}

	void GLRenderDevice::LoadShader(ShaderResource* shader)
	{
		const std::string& path = shader->GetFilePath();
		const GLchar* data = FileLoader::Read(shader->GetFilePath(), FileLoader::E_ROOT, true);

		int shaderType;

		if (Contains(path, ".vs."))			shaderType = GL_VERTEX_SHADER;
		else if (Contains(path, ".ps."))	shaderType = GL_FRAGMENT_SHADER;
		else if (Contains(path, ".gs."))	shaderType = GL_GEOMETRY_SHADER;
		else if (Contains(path, ".cs."))	shaderType = GL_COMPUTE_SHADER;
		else if (Contains(path, ".tc."))	shaderType = GL_TESS_CONTROL_SHADER;
		else if (Contains(path, ".te."))	shaderType = GL_TESS_EVALUATION_SHADER;
		else								return;

		// Create an empty vertex shader handle
		GLuint shaderHandle = glCreateShader(shaderType);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		glShaderSource(shaderHandle, 1, &data, 0);

		// Compile the vertex shader
		glCompileShader(shaderHandle);

		GLint isCompiled = 0;
		glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &isCompiled);
		delete[] data;

		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shaderHandle, maxLength, &maxLength, &infoLog[0]);

			CB_CORE_WARN("Could not compile shader, output:\n{0}", infoLog.data());

			// We don't need the shader anymore.
			glDeleteShader(shaderHandle);

			// In this simple program, we'll just leave
			return;
		}

		CB_CORE_INFO("Succesfully compiled shader at path \"{0}\"", path);
		shader->SetHandle(shaderHandle);
	}

	void GLRenderDevice::UnloadShader(ShaderResource* shader)
	{
		glDeleteShader(shader->GetHandle());
		CB_CORE_INFO("Succesfully unloaded shader at path \"{0}\"", shader->GetFilePath());
	}

	Shader* GLRenderDevice::CreateShaderProgram(const Shader::Descriptor& descriptor)
	{
		GLShader* shader = new GLShader(descriptor);

		if (shader->IsLinked())
		{
			m_Shaders.push_back(shader);
			return shader;
		}

		delete shader;
		return nullptr;
	}
}

#endif