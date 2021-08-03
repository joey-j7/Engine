#pragma once

#include "Engine/Files/Rendering/ShaderFile.h"
#include <glad/glad.h>

namespace Engine
{
	class Engine_API GLShaderFile : public ShaderFile
	{
	public:
		GLShaderFile(const String& filePath);
		virtual ~GLShaderFile();

		uint32_t GetHandle() const { return m_uiHandle; };

	private:
		uint32_t m_uiHandle = UINT_MAX;
		GLenum m_GLType = GL_VERTEX_SHADER;
	};
}