#pragma once

#include "Engine/Objects/Object.h"

#include <include/core/SkSurface.h>
#include "Engine/Objects/Renderable.h"

namespace Engine
{
	class Engine_API Renderer : public Object
	{
		friend class UIRenderable;
		friend class Renderable;

	public:
		Renderer();
		virtual ~Renderer();

		void AddModifiedComponent(Renderable& Component);
		
		virtual void Swap();
		virtual void Present() = 0;

	protected:
		std::vector<Renderable*> m_Components;
	};
}
