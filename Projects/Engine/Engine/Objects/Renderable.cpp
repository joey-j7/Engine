#include "pch.h"
#include "Renderable.h"

#include "Engine/Application.h"

namespace Engine
{
	Renderable::Renderable()
	{

	}

	Renderable::~Renderable()
	{
		auto Find = std::find(m_Renderer->m_Components.begin(), m_Renderer->m_Components.end(), this);

		if (Find != m_Renderer->m_Components.end())
			m_Renderer->m_Components.erase(Find);
	}

	void Renderable::MarkDirty()
	{
		if (m_RenderState == E_MODIFIED)
			return;

		m_RenderState = E_MODIFIED;
		m_Renderer->AddModifiedComponent(*this);
	}
}
