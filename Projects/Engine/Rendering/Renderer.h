#pragma once

#include <memory>
#include <vector>

#include "Engine/Resources/ResourceDatabase.h"

#include "Rendering/RenderContextData.h"
#include "Rendering/RenderDevice.h"

namespace Engine {
	class Engine_API Renderer
	{
	public:
		Renderer(const std::shared_ptr<RenderContextData>& contextData);
		virtual ~Renderer()
		{
			// Free objects before ResourceDatabase's resources (because they are based on it)
			if (m_pRenderDevice)
				m_pRenderDevice->Free();
		}

		virtual void Clear() = 0;
		static Renderer* Create(const std::shared_ptr<RenderContextData>& contextData);

		virtual void Verify(int err) {};

		virtual RenderResource* Load(const std::string& filePath, FileLoader::Type pathType = FileLoader::Type::E_CONTENT);
		virtual bool Unload(RenderResource* resource);

		RenderDevice& GetRenderDevice() { return *m_pRenderDevice; }

	protected:
		static const char* m_Name;

		std::unique_ptr<RenderDevice> m_pRenderDevice = nullptr;
		std::shared_ptr<RenderContextData> m_pContextData = nullptr;
		std::unique_ptr<ResourceDatabase> m_pDatabase = nullptr;
	};

}