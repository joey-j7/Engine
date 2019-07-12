#pragma once

#include "Rendering/RenderDatabase.h"

namespace Engine
{
	class Engine_API GLRenderDevice : public RenderDatabase
	{
	public:
		virtual void LoadModel(ModelResource* model) override;
		virtual void UnloadModel(ModelResource* model) override;

		virtual void LoadTexture(TextureResource* texture) override;
		virtual void UnloadTexture(TextureResource* texture) override;

		virtual void LoadShader(ShaderResource* shader) override;
		virtual void UnloadShader(ShaderResource* shader) override;

		virtual Shader* CreateShaderProgram(const Shader::Descriptor& descriptor) override;
	};
}