#include "pch.h"
#include "Renderer.h"

namespace Engine
{
	Renderer::Renderer()
	{
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::AddModifiedComponent(RenderComponent& Component)
	{
		const auto Find = std::find(m_Components.begin(), m_Components.end(), & Component);

		if (Find != m_Components.end())
		{
			assert(false);
			return;
		}

		m_Components.push_back(&Component);
	}

	void Renderer::Swap()
	{
		m_Components.clear();
	}
}
