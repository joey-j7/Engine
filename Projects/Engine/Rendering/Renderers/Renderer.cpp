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

	void Renderer::AddModifiedComponent(Renderable& Renderable)
	{
		const auto Find = std::find(m_Components.begin(), m_Components.end(), &Renderable);

		if (Find != m_Components.end())
		{
			assert(false);
			return;
		}

		m_Components.push_back(&Renderable);
	}

	void Renderer::Swap()
	{

	}
}
