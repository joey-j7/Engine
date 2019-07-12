#include "pch.h"
#include "RenderAPI.h"

#include "Resources/ModelResource.h"
#include "Resources/TextureResource.h"
#include "Resources/ShaderResource.h"

namespace Engine
{
	RenderAPI::RenderAPI(const std::shared_ptr<RenderContextData>& contextData)
	{
		m_pContextData = contextData;

		m_pDatabase = std::make_unique<ResourceDatabase>();
		m_pDatabase->Add<ModelResource>({ "fbx", "obj", "vox" });
		m_pDatabase->Add<TextureResource>({ "png", "jpg", "bmp" });
		m_pDatabase->Add<ShaderResource>({ "glsl" });

		CB_CORE_INFO("{0} rendering API has been initialized", m_Name);
	}

	RenderResource* RenderAPI::Load(const std::string& filePath, FileLoader::Type pathType)
	{
		if (RenderResource* p = static_cast<RenderResource*>(m_pDatabase->Load(filePath, pathType)))
		{
			return p;
		}

		CB_CORE_WARN("Tried to load an unsupported file {0}", filePath);
		return nullptr;
	}

	bool RenderAPI::Unload(RenderResource* resource)
	{
		return m_pDatabase->Unload(resource);
	}
}