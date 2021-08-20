#pragma once

#include "Engine/Objects/Worlds/Entities/StaticEntity.h"

#include "Engine/Objects/Worlds/Entities/Components/ClickableComponent.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Elements/UIText.h"

namespace Engine
{
	class UIRect;

	struct ButtonStyle
	{
		String m_Text = "";
		
		float m_Radius = 20.f;
		Vector4 m_Padding = Vector4(20.f);

		String m_BackgroundImagePath = "";
		String m_ForegroundImagePath = "";
		
		Color m_Color = SK_ColorWHITE;
		UIElement::Gradient m_Gradient;
	};
	
	class Engine_API UIButton : public StaticEntity
	{
	public:
		UIButton(
			const ButtonStyle& DefaultStyle = ButtonStyle(),
			const ButtonStyle& HoverStyle = ButtonStyle(),
			const ButtonStyle& PressStyle = ButtonStyle(),
			const String& sName = "Button"
		);

		void SetAnchor(Anchor NewAnchor);

		void SetOnEnterCallback(const std::function<void()>& Enter);
		void SetOnExitCallback(const std::function<void()>& Exit);
		void SetOnPressedCallback(const std::function<void()>& Pressed);
		void SetOnReleasedCallback(const std::function<void()>& Released);
		void SetOnClickedCallback(const std::function<void()>& Clicked);
		void SetOnDraggedCallback(const std::function<void(const DVector2&)>& Dragged);

	protected:
		virtual void OnEnter(const DVector2& Position);
		virtual void OnExit(const DVector2& Position);
		virtual void OnDragged(const DVector2& Position, const DVector2& Delta);

		virtual void OnPressed();
		virtual void OnReleased();

		virtual void OnClicked();

		void SetText(const String& Text);
		void ApplyStyle(const ButtonStyle& NewStyle);

		std::function<void()> m_OnEnter;
		std::function<void()> m_OnExit;
		std::function<void()> m_OnPressed;
		std::function<void()> m_OnReleased;
		std::function<void()> m_OnClicked;
		std::function<void(const DVector2&)> m_OnDragged;
		
		ClickableComponent* m_ClickableComponent = nullptr;
		UIRect* m_BackgroundComponent = nullptr;
		UIImage* m_BackgroundImageComponent = nullptr;
		UIImage* m_ForegroundImageComponent = nullptr;
		UIText* m_TextComponent = nullptr;

		Anchor m_Anchor = E_ANCH_CENTER;

		std::unique_ptr<StaticEntity> m_ForegroundEntity = nullptr;

		ButtonStyle m_DefaultStyle;
		ButtonStyle m_HoverStyle;
		ButtonStyle m_PressStyle;
	};
}
