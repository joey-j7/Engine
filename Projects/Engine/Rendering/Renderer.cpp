#include "pch.h"
#include "Renderer.h"

#include "Resources/ModelResource.h"
#include "Resources/TextureResource.h"
#include "Resources/ShaderResource.h"

namespace Engine
{
	Renderer::Renderer(const std::shared_ptr<RenderContextData>& contextData)
	{
		m_ContextData = contextData;

		m_Database.Add<ModelResource>({ "fbx", "obj", "vox" });
		m_Database.Add<TextureResource>({ "png", "jpg", "bmp" });
		m_Database.Add<ShaderResource>({ "glsl" });

		CB_CORE_INFO("{0} rendering API has been initialized", m_Name);
	}

	uint32_t Renderer::Load(const std::string& filePath, FileLoader::Type pathType)
	{
		if (ResourceType* p = m_Database.Load(filePath, pathType))
		{
			return 0;
		}

		CB_CORE_WARN("Tried to load an unsupported file {0}", filePath);
		return -1;
	}
}