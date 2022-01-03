#include "pch.h"
#include "UIButton.h"

#include "Engine/Objects/Components/ClickableComponent.h"
#include "Engine/Objects/Components/UI/Renderables/UIImage.h"
#include "Engine/Objects/Components/UI/Renderables/Shapes/UIRect.h"

namespace Engine
{
	UIButton::UIButton(
		World& World,
		const ButtonStyle& DefaultStyle,
		const ButtonStyle& HoverStyle,
		const ButtonStyle& PressStyle,
		const String& sName
	) : Entity(World, sName), m_DefaultStyle(DefaultStyle),
	m_HoverStyle(HoverStyle), m_PressStyle(PressStyle)
	{
		m_ClickableComponent = AddComponent<ClickableComponent>();
		
		m_ClickableComponent->OnEnterEvent.Bind(this, &UIButton::OnEnter);
		m_ClickableComponent->OnExitEvent.Bind(this, &UIButton::OnExit);
		m_ClickableComponent->OnPressedEvent.Bind(this, &UIButton::OnPressed);
		m_ClickableComponent->OnReleasedEvent.Bind(this, &UIButton::OnReleased);

		m_ClickableComponent->OnClickedEvent.Bind(this, &UIButton::OnClicked);
		m_ClickableComponent->OnDraggedEvent.Bind(this, &UIButton::OnDragged);

		m_BackgroundComponent = AddComponent<UIRect>();

		m_ForegroundEntity = GetWorld().Add<Entity>("Button Foreground");
		m_ForegroundEntity->SetParent(this);

		SetPivot(m_Pivot);
		SetAnchor(m_Anchor);
		ApplyStyle(m_DefaultStyle);
	}

	void UIButton::SetPivot(const Vector2& Pivot)
	{
		m_Pivot = Pivot;

		m_BackgroundComponent->SetPivot(m_Pivot);

		if (m_BackgroundImageComponent)
			m_BackgroundImageComponent->SetPivot(m_Pivot);
	}

	void UIButton::SetAnchor(Anchor NewAnchor)
	{
		m_Anchor = NewAnchor;

		m_BackgroundComponent->SetAnchor(m_Anchor);

		if (m_BackgroundImageComponent)
			m_BackgroundImageComponent->SetAnchor(m_Anchor);
	}

	void UIButton::Click()
	{
		m_ClickableComponent->Click();
	}

	void UIButton::SetOnEnterCallback(const std::function<void()>& Enter)
	{
		m_OnEnter = Enter;
	}

	void UIButton::SetOnExitCallback(const std::function<void()>& Exit)
	{
		m_OnExit = Exit;
	}

	void UIButton::SetOnPressedCallback(const std::function<void()>& Pressed)
	{
		m_OnPressed = Pressed;
	}

	void UIButton::SetOnReleasedCallback(const std::function<void()>& Released)
	{
		m_OnReleased = Released;
	}

	void UIButton::SetOnClickedCallback(const std::function<void()>& Clicked)
	{
		m_OnClicked = Clicked;
	}

	void UIButton::SetOnDraggedCallback(const std::function<void(const DVector2&)>& Dragged)
	{
		m_OnDragged = Dragged;
	}

	void UIButton::SetDefaultStyle(const ButtonStyle& Style)
	{
		m_DefaultStyle = Style;

		if (!m_ClickableComponent->IsHovered() && !m_ClickableComponent->IsPressed())
			ApplyStyle(Style);
	}

	void UIButton::SetHoverStyle(const ButtonStyle& Style)
	{
		m_HoverStyle = Style;

		if (m_ClickableComponent->IsHovered() && !m_ClickableComponent->IsPressed())
			ApplyStyle(Style);
	}

	void UIButton::SetPressStyle(const ButtonStyle& Style)
	{
		m_PressStyle = Style;

		if (m_ClickableComponent->IsPressed())
			ApplyStyle(Style);
	}

	void UIButton::ApplyText(const String& Text)
	{
		if (!m_TextComponent && Text.empty())
			return;

		if (!m_TextComponent)
		{
			m_TextComponent = m_ForegroundEntity->AddComponent<UIText>();
			
			m_TextComponent->SetPivot(Vector2(0.5f, 0.5f));
			m_TextComponent->SetAnchor(E_ANCH_CENTER);
		}
		
		if (m_TextComponent->GetText() == Text)
			return;

		m_TextComponent->SetText(Text);

		const AABB Bounds = m_TextComponent->GetUnscaledBounds();

		m_BackgroundComponent->SetSize(
			static_cast<uint32_t>(Bounds.width()),
			static_cast<uint32_t>(Bounds.height())
		);
	}

	void UIButton::SetText(const String& Text)
	{
		m_DefaultStyle.m_Text = Text;
		m_HoverStyle.m_Text = Text;
		m_PressStyle.m_Text = Text;

		ApplyStyle(*m_CurrentStyle);
	}

	void UIButton::OnEnter(const DVector2& Position)
	{
		if (!m_ClickableComponent->IsPressed())
			ApplyStyle(m_HoverStyle);

		if (m_OnEnter)
			m_OnEnter();
	}

	void UIButton::OnExit(const DVector2& Position)
	{
		if (!m_ClickableComponent->IsPressed())
			ApplyStyle(m_DefaultStyle);

		if (m_OnExit)
			m_OnExit();
	}

	void UIButton::OnPressed()
	{
		ApplyStyle(m_PressStyle);

		if (m_OnPressed)
			m_OnPressed();
	}

	void UIButton::OnReleased()
	{
		if (m_ClickableComponent->IsHovered())
			ApplyStyle(m_HoverStyle);
		else
			ApplyStyle(m_DefaultStyle);

		if (m_OnReleased)
			m_OnReleased();
	}
	
	void UIButton::OnClicked()
	{
		if (m_OnClicked)
			m_OnClicked();
	}

	void UIButton::OnDragged(const DVector2& Position, const DVector2& Delta)
	{
		if (m_OnDragged)
			m_OnDragged(Delta);
	}

	void UIButton::ApplyStyle(const ButtonStyle& NewStyle)
	{
		m_CurrentStyle = &NewStyle;

		if (NewStyle.m_BorderWidth > 0 &&
			NewStyle.m_BorderColor != Color(0.f)
		)
		{
			m_BackgroundComponent->SetBorder(
				NewStyle.m_BorderWidth,
				NewStyle.m_BorderColor,
				NewStyle.m_ShowFill
			);
		}

		if (!NewStyle.m_BackgroundImagePath.empty() || m_BackgroundImageComponent)
		{
			if (!m_BackgroundImageComponent)
			{
				m_BackgroundImageComponent = AddComponent<UIImage>();
				m_BackgroundImageComponent->SetPivot(m_Pivot);
				m_BackgroundImageComponent->SetAnchor(m_Anchor);
			}
			
			m_BackgroundImageComponent->SetImage(
				NewStyle.m_ShowFill ?
				NewStyle.m_BackgroundImagePath :
				""
			);
		}

		if (!NewStyle.m_ForegroundImagePath.empty() || m_ForegroundImageComponent)
		{
			if (!m_ForegroundImageComponent)
			{
				m_ForegroundImageComponent = m_ForegroundEntity->AddComponent<UIImage>();

				m_ForegroundImageComponent->SetPivot(Vector2(0.5f, 0.5f));
				m_ForegroundImageComponent->SetAnchor(E_ANCH_CENTER);
			}
			
			m_ForegroundImageComponent->SetImage(
				NewStyle.m_ShowFill ?
				NewStyle.m_ForegroundImagePath :
				""
			);
		}
		
		if (NewStyle.m_Gradient.HasColors())
		{
			m_BackgroundComponent->SetGradient(NewStyle.m_Gradient);
		}
		else
		{
			m_BackgroundComponent->SetColor(NewStyle.m_Color);
		}

		m_BackgroundComponent->SetMinSize(NewStyle.m_MinSize);
		m_BackgroundComponent->SetRadius(NewStyle.m_Radius);
		
		ApplyText(NewStyle.m_Text);
	}
}
