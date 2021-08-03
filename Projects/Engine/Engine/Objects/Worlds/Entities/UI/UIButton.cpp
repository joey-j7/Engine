#include "pch.h"
#include "UIButton.h"

#include "Engine/Objects/Worlds/Entities/Components/ClickableComponent.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Elements/UIImage.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Elements/Shapes/UIRect.h"

namespace Engine
{
	UIButton::UIButton(
		std::function<void()> Function,
		const Style& DefaultStyle,
		const Style& HoverStyle,
		const Style& PressStyle,
		const String& sName
	) : StaticEntity(sName), m_DefaultStyle(DefaultStyle),
	m_HoverStyle(HoverStyle), m_PressStyle(PressStyle), m_Function(Function)
	{
		m_ClickableComponent = AddComponent<ClickableComponent>();
		
		m_ClickableComponent->OnEnterEvent.Bind(this, &UIButton::OnEnter);
		m_ClickableComponent->OnExitEvent.Bind(this, &UIButton::OnExit);
		m_ClickableComponent->OnPressedEvent.Bind(this, &UIButton::OnPressed);
		m_ClickableComponent->OnReleasedEvent.Bind(this, &UIButton::OnReleased);
		
		m_ClickableComponent->OnClickedEvent.Bind(this, &UIButton::OnClicked);

		m_BackgroundComponent = AddComponent<UIRect>();
		m_BackgroundComponent->SetAlignment(Vector2(0.5f, 0.5f));
		m_BackgroundComponent->SetAnchor(E_ANCH_CENTER);

		m_ForegroundEntity = std::make_unique<StaticEntity>("Button Foreground");
		m_ForegroundEntity->SetParent(this);

		ApplyStyle(m_DefaultStyle);
	}

	void UIButton::SetText(const String& Text)
	{
		if (!m_TextComponent && Text.empty())
			return;

		if (!m_TextComponent)
		{
			m_TextComponent = AddComponent<UIText>();
			
			m_TextComponent->SetAlignment(Vector2(0.5f, 0.5f));
			m_TextComponent->SetAnchor(E_ANCH_CENTER);
		}
		
		if (m_TextComponent->GetText() == Text)
			return;

		m_TextComponent->SetText(Text);

		const AABB Bounds = m_TextComponent->GetUnscaledBounds();
		const Vector2 Size = Vector2(
			Bounds.fRight - Bounds.fLeft,
			Bounds.fBottom - Bounds.fTop
		);

		m_BackgroundComponent->SetSize(
			static_cast<uint32_t>(Size.x),
			static_cast<uint32_t>(Size.y)
		);
	}

	void UIButton::OnEnter(const DVector2& Position)
	{
		if (!m_ClickableComponent->IsPressed())
			ApplyStyle(m_HoverStyle);
	}

	void UIButton::OnExit(const DVector2& Position)
	{
		if (!m_ClickableComponent->IsPressed())
			ApplyStyle(m_DefaultStyle);
	}

	void UIButton::OnPressed()
	{
		ApplyStyle(m_PressStyle);
	}

	void UIButton::OnReleased()
	{
		if (m_ClickableComponent->IsHovered())
			ApplyStyle(m_HoverStyle);
		else
			ApplyStyle(m_DefaultStyle);
	}
	
	void UIButton::OnClicked()
	{
		if (m_Function)
			m_Function();
		
		SetText(
			m_TextComponent->GetText() == "Clicked!" ? "Unclicked!" : "Clicked!"
		);
	}
	void UIButton::ApplyStyle(const Style& NewStyle)
	{
		if (!NewStyle.m_BackgroundImagePath.empty() || m_BackgroundImageComponent)
		{
			if (!m_BackgroundImageComponent)
				m_BackgroundImageComponent = AddComponent<UIImage>();
			
			m_BackgroundImageComponent->SetImage(NewStyle.m_BackgroundImagePath);
		}

		if (!NewStyle.m_ForegroundImagePath.empty() || m_ForegroundImageComponent)
		{
			if (!m_ForegroundImageComponent)
				m_ForegroundImageComponent = m_ForegroundEntity->AddComponent<UIImage>();

			m_ForegroundImageComponent->SetImage(NewStyle.m_ForegroundImagePath);
		}
		
		if (NewStyle.m_Gradient.HasColors())
		{
			m_BackgroundComponent->SetGradient(NewStyle.m_Gradient);
		}
		else
		{
			m_BackgroundComponent->SetColor(NewStyle.m_Color);
		}

		m_BackgroundComponent->SetPadding(NewStyle.m_Padding);
		m_BackgroundComponent->SetRadius(NewStyle.m_Color);
		
		SetText(NewStyle.m_Text);
	}
}
