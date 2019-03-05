#pragma once

#include "Platform/Window.h"
#include "Rendering/Renderer.h"

#include "Engine/Resources/ResourceManager.h"

#include "Rendering/Resources/ShaderResource.h"
#include "Rendering/Resources/TextureResource.h"
#include "Rendering/Resources/ModelResource.h"

#include "RenderContextData.h"

namespace Engine {
	class RenderContext
	{
	public:
		void Init();

		// Upload
		void Upload(const std::string& m_FilePath);

		Window& GetWindow() const { return *m_Window; };
		Renderer& GetRenderer() const { return *m_Renderer; };
		RenderContextData& GetData() const { return *m_ContextData; };

	protected:
		std::unique_ptr<Window> m_Window;
		std::unique_ptr<Renderer> m_Renderer;
		std::shared_ptr<RenderContextData> m_ContextData;

		std::unique_ptr<ResourceManager<ShaderResource>> m_Shaders;
		std::unique_ptr<ResourceManager<TextureResource>> m_Textures;
		std::unique_ptr<ResourceManager<ModelResource>> m_Models;

	private:
		bool m_Initialized = false;
	};
}