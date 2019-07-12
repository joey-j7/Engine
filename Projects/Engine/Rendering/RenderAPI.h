#pragma once

#include <memory>

#include "Engine/Resources/ResourceDatabase.h"
#include "Rendering/RenderContextData.h"

namespace Engine
{
	class Engine_API RenderAPI
	{
	public:
		RenderAPI(const std::shared_ptr<RenderContextData>& contextData);
		virtual ~RenderAPI()
		{

		}

		virtual void Clear() = 0;
		static RenderAPI* Create(const std::shared_ptr<RenderContextData>& contextData);

		virtual void Verify(int err) {};

		virtual RenderResource* Load(const std::string& filePath, FileLoader::Type pathType = FileLoader::Type::E_CONTENT);
		virtual bool Unload(RenderResource* resource);

	protected:
		static const char* m_Name;

		std::shared_ptr<RenderContextData> m_pContextData = nullptr;
		std::unique_ptr<ResourceDatabase> m_pDatabase = nullptr;
	};

}