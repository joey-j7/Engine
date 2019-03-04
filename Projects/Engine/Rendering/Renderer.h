#pragma once

#include <memory>
#include "Rendering/RenderContextData.h"

namespace Engine {
	class Engine_API Renderer
	{
	public:
		Renderer(const std::shared_ptr<RenderContextData>& contextData) {
			CB_CORE_INFO_T(5.0f, "{0} rendering API has been initialized", m_Name);
			m_ContextData = contextData;
		};

		virtual ~Renderer() {}

		virtual void Clear() = 0;
		static Renderer* Create(const std::shared_ptr<RenderContextData>& contextData);

		virtual void Verify(int err) {};

		static const char* m_Name;

	protected:
		std::shared_ptr<RenderContextData> m_ContextData = nullptr;
	};

}