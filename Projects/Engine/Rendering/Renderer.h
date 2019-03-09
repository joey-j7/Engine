#pragma once

#include <memory>

#include "Rendering/RenderContextData.h"
#include "Engine/Resources/ResourceDatabase.h"

namespace Engine {
	class Engine_API Renderer
	{
	public:
		Renderer(const std::shared_ptr<RenderContextData>& contextData);

		virtual ~Renderer() {}

		virtual void Clear() = 0;
		static Renderer* Create(const std::shared_ptr<RenderContextData>& contextData);

		virtual void Verify(int err) {};

		virtual uint32_t Load(const std::string& filePath, FileLoader::Type pathType = FileLoader::Type::E_CONTENT);

	protected:
		static const char* m_Name;
		std::shared_ptr<RenderContextData> m_ContextData = nullptr;

		ResourceDatabase m_Database;
	};

}