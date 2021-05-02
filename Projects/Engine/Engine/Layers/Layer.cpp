#include "pch.h"
#include "Layer.h"

namespace Engine
{
	Layer::Layer(const std::string& name) : DynamicObject(name)
	{
	}

	Layer::~Layer()
	{

	}

	void Layer::OnAttach(const LayerStack& stack)
	{
		m_uiAttachCount++;
	}

	bool Layer::OnDetach(const LayerStack& stack)
	{
		if (m_uiAttachCount > 0)
		{
			m_uiAttachCount--;
			return true;
		}

		return false;
	}
}
