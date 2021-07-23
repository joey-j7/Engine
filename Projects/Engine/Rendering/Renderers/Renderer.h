#pragma once

#include "Engine/Objects/Object.h"

#include <include/gpu/GrDirectContext.h>
#include <include/core/SkSurface.h>
#include <include/core/SkCanvas.h>

#include "Engine/Objects/Worlds/Entities/Components/RenderComponent.h"

namespace Engine
{
	class Engine_API Renderer : public Object
	{
		friend class UIComponent;

	public:
		Renderer();
		virtual ~Renderer();

		void AddModifiedComponent(RenderComponent& Component);
		
		virtual void Swap();
		virtual void Present() = 0;

	protected:
		std::vector<RenderComponent*> m_Components;
	};
}
