#include "pch.h"
#include "ClickableComponent.h"

#include "Engine/Objects/Worlds/Entities/Entity.h"

#include "Transform/TransformComponent2D.h"
#include "Transform/TransformComponent3D.h"

#include "UI/UIText.h"

namespace Engine
{
	ClickableComponent::ClickableComponent(Entity& Entity, const std::string& sName) : Component(Entity, sName), Clickable()
	{
		if (!GetEntity().GetComponent<TransformComponent3D>())
		{
			AddDependencyTypes<TransformComponent2D>();
		}
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

	void ClickableComponent::OnPressed()
	{
		Clickable::OnPressed();
		
		m_Entity.GetComponent<TransformComponent2D>()->SetScale(Vector2(0.5f));
	}

	void ClickableComponent::OnReleased()
	{
		Clickable::OnReleased();

		m_Entity.GetComponent<TransformComponent2D>()->SetScale(Vector2(1.f));
	}
}
