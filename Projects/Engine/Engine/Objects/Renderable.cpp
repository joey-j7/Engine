#include "pch.h"
#include "Renderable.h"

#include "Engine/Application.h"

namespace Engine
{
	Renderable::Renderable()
	{

	}

	void Renderable::MarkDirty()
	{
		if (m_RenderState == E_MODIFIED)
			return;

		m_RenderState = E_MODIFIED;
		m_Renderer->AddModifiedComponent(*this);
	}
}
