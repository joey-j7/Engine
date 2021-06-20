#include "pch.h"
#include "RenderAPI.h"


#include "Engine/Application.h"
#include "Engine/Files/Rendering/ModelFile.h"
#include "Engine/Files/Rendering/TextureFile.h"
#include "Engine/Files/Rendering/ShaderFile.h"

namespace Engine
{
	RenderAPI::RenderAPI(RenderContext& renderContext) : Object("Unknown")
	{
		m_pRenderContext = &renderContext;
	}

	bool RenderAPI::Init()
	{
		if (m_bInitialized)
			return false;

		m_bInitialized = true;
		m_bRunning = true;

		CB_CORE_INFO("{0} rendering API has been initialized", GetName());

		return true;
	};

	bool RenderAPI::Deinit()
	{
		if (!m_bInitialized)
			return false;

		m_bInitialized = false;
		m_bRunning = false;

		CB_CORE_INFO("{0} rendering API has been deinitialized", GetName());

		return true;
	}

	void RenderAPI::Reset()
	{
		Deinit();
		Init();
	}

	RenderFile* RenderAPI::Load(const std::string& filePath, FileLoader::Type pathType)
	{
		if (RenderFile* p = static_cast<RenderFile*>(FileDatabase::Get().Load(filePath, pathType)))
		{
			return p;
		}

		CB_CORE_WARN("Tried to load an unsupported file \"{0}\"", filePath);
		return nullptr;
	}

	bool RenderAPI::Unload(RenderFile* resource)
	{
		return FileDatabase::Get().Unload(resource);
	}
}
