#include "pch.h"
#include "ClickableComponent.h"

#include "Engine/Objects/Worlds/Entities/Entity.h"
#include "UI/UIText.h"

namespace Engine
{
	ClickableComponent::ClickableComponent(Entity& Entity, const std::string& sName) : Component(Entity, sName), Clickable()
	{
	}

	const AABB ClickableComponent::GetBounds() const
	{
		return m_Entity.GetBounds();
	}

	void ClickableComponent::OnEnter(double XPosition, double YPosition)
	{
		Clickable::OnEnter(XPosition, YPosition);

		if (m_Entity.GetComponent<UIText>())
		m_Entity.GetComponent<UIText>()->SetColor(SK_ColorRED);
	}

	void ClickableComponent::OnExit(double XPosition, double YPosition)
	{
		Clickable::OnExit(XPosition, YPosition);

		if (m_Entity.GetComponent<UIText>())
		m_Entity.GetComponent<UIText>()->SetColor(SK_ColorBLUE);
	}
}
