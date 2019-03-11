#pragma once

#include "Shader.h"
#include <vector>

namespace Engine
{
	class ModelResource;
	class TextureResource;
	class ShaderResource;

	class Engine_API RenderDevice
	{
		friend class Renderer;

	public:
		virtual ~RenderDevice();

		virtual void LoadModel(ModelResource* model) = 0;
		virtual void UnloadModel(ModelResource* model) = 0;

		virtual void LoadTexture(TextureResource* texture) = 0;
		virtual void UnloadTexture(TextureResource* texture) = 0;

		virtual void LoadShader(ShaderResource* shader) = 0;
		virtual void UnloadShader(ShaderResource* shader) = 0;

		virtual Shader* CreateShaderProgram(const Shader::Descriptor& descriptor) = 0;

	protected:
		void Free();
		std::vector<Shader*> m_Shaders;
	};
}