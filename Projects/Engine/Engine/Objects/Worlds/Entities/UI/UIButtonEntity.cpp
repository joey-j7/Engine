#include "pch.h"
#include "UIButtonEntity.h"

#include "Engine/Objects/Worlds/Entities/Components/ClickableComponent.h"

namespace Engine
{
	UIButtonEntity::UIButtonEntity(
		const std::string& Text,
		const std::string& sName
	) : StaticEntity(sName)
	{
		m_ClickableComponent = AddComponent<ClickableComponent>();
		m_RectangleComponent = AddComponent<UIRect>();
		m_TextComponent = AddComponent<UIText>();
		
		m_ClickableComponent->OnEnterEvent.Bind(this, &UIButtonEntity::OnEnter);
		m_ClickableComponent->OnExitEvent.Bind(this, &UIButtonEntity::OnExit);
		m_ClickableComponent->OnPressedEvent.Bind(this, &UIButtonEntity::OnPressed);
		m_ClickableComponent->OnReleasedEvent.Bind(this, &UIButtonEntity::OnReleased);
		
		m_ClickableComponent->OnClickedEvent.Bind(this, &UIButtonEntity::OnClicked);

		m_TextComponent->SetPadding(Vector4(20.f));
		m_RectangleComponent->SetColor(SK_ColorGREEN);

		m_TextComponent->SetAlignment(Vector2(0.5f, 0.5f));
		m_TextComponent->SetAnchor(E_ANCH_CENTER);

		m_RectangleComponent->SetAlignment(Vector2(0.5f, 0.5f));
		m_RectangleComponent->SetAnchor(E_ANCH_CENTER);
		
		SetText(Text);	
	}

	void UIButtonEntity::SetText(const std::string& Text)
	{
		if (m_TextComponent->GetText() == Text)
			return;

		m_TextComponent->SetText(Text);

		const AABB Bounds = m_TextComponent->GetUnscaledBounds();
		const Vector2 Size = Vector2(
			Bounds.fRight - Bounds.fLeft,
			Bounds.fBottom - Bounds.fTop
		);

		m_RectangleComponent->SetSize(
			static_cast<uint32_t>(Size.x),
			static_cast<uint32_t>(Size.y)
		);

		m_RectangleComponent->SetRadius(10.f);
	}

	void UIButtonEntity::OnEnter(const DVector2& Position)
	{
		if (!m_ClickableComponent->IsPressed())
			m_RectangleComponent->SetColor(SK_ColorYELLOW);
	}

	void UIButtonEntity::OnExit(const DVector2& Position)
	{
		if (!m_ClickableComponent->IsPressed())
			m_RectangleComponent->SetColor(SK_ColorGREEN);
	}

	void UIButtonEntity::OnPressed()
	{
		m_RectangleComponent->SetColor(SK_ColorRED);
	}

	void UIButtonEntity::OnReleased()
	{
		if (m_ClickableComponent->IsHovered())
			m_RectangleComponent->SetColor(SK_ColorYELLOW);
		else
			m_RectangleComponent->SetColor(SK_ColorGREEN);
	}
	
	void UIButtonEntity::OnClicked()
	{
		SetText(
			m_TextComponent->GetText() == "Clicked!" ? "Unclicked!" : "Clicked!"
		);
	}
}
