#include "pch.h"
#include "RenderAPI.h"

#include "Engine/Files/Rendering/ModelFile.h"
#include "Engine/Files/Rendering/TextureFile.h"
#include "Engine/Files/Rendering/ShaderFile.h"

namespace Engine
{
	RenderAPI::RenderAPI(RenderContext& renderContext) : Object("Unknown")
	{
		m_pRenderContext = &renderContext;
		m_pDatabase = std::make_unique<FileDatabase>();
	}

	bool RenderAPI::Init()
	{
		if (m_bInitialized)
			return false;

		m_bInitialized = true;
		CB_CORE_INFO("{0} rendering API has been initialized", GetName());

		return true;
	};

	RenderFile* RenderAPI::Load(const std::string& filePath, FileLoader::Type pathType)
	{
		if (RenderFile* p = static_cast<RenderFile*>(m_pDatabase->Load(filePath, pathType)))
		{
			return p;
		}

		CB_CORE_WARN("Tried to load an unsupported file \"{0}\"", filePath);
		return nullptr;
	}

	bool RenderAPI::Unload(RenderFile* resource)
	{
		return m_pDatabase->Unload(resource);
	}
}
