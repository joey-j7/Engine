#pragma once

#include "Engine/Objects/Components/ClickableComponent.h"
#include "Engine/Objects/Components/UI/Renderables/UIText.h"

namespace Engine
{
	class UIRect;

	struct ButtonStyle
	{
		String m_Text = "";
		
		float m_Radius = 20.f;
		Vector4 m_MinSize = Vector4(20.f);

		String m_BackgroundImagePath = "";
		String m_ForegroundImagePath = "";
		
		Color m_Color = Color(1.f);
		UIRenderable::Gradient m_Gradient;

		uint32_t m_BorderWidth = 0;
		Color m_BorderColor = Color(1.f);

		bool m_ShowFill = true;
	};
	
	class Engine_API UIButton : public Entity
	{
	public:
		UIButton(
			World& World,
			const ButtonStyle& DefaultStyle = ButtonStyle(),
			const ButtonStyle& HoverStyle = ButtonStyle(),
			const ButtonStyle& PressStyle = ButtonStyle(),
			const String& sName = "Button"
		);

		void SetPivot(const Vector2& Pivot);
		void SetAnchor(Anchor NewAnchor);

		void Click();

		Entity* GetForeground() const { return m_ForegroundEntity; }

		void SetOnEnterCallback(const std::function<void()>& Enter);
		void SetOnExitCallback(const std::function<void()>& Exit);
		void SetOnPressedCallback(const std::function<void()>& Pressed);
		void SetOnReleasedCallback(const std::function<void()>& Released);
		void SetOnClickedCallback(const std::function<void()>& Clicked);
		void SetOnDraggedCallback(const std::function<void(const DVector2&)>& Dragged);

		const ButtonStyle& GetDefaultStyle() const { return m_DefaultStyle; };
		void SetDefaultStyle(const ButtonStyle& Style);

		const ButtonStyle& GetHoverStyle() const { return m_HoverStyle; };
		void SetHoverStyle(const ButtonStyle& Style);

		const ButtonStyle& GetPressStyle() const { return m_PressStyle; };
		void SetPressStyle(const ButtonStyle& Style);

		void SetText(const String& Text);

	protected:
		virtual void OnEnter(const DVector2& Position);
		virtual void OnExit(const DVector2& Position);
		virtual void OnDragged(const DVector2& Position, const DVector2& Delta);

		virtual void OnPressed();
		virtual void OnReleased();

		virtual void OnClicked();

		void ApplyText(const String& Text);
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

		Anchor m_Anchor = E_ANCH_TOP_LEFT;
		Vector2 m_Pivot = Vector2(0.f, 0.f);

		Entity* m_ForegroundEntity = nullptr;

		ButtonStyle m_DefaultStyle;
		ButtonStyle m_HoverStyle;
		ButtonStyle m_PressStyle;

		const ButtonStyle* m_CurrentStyle = &m_DefaultStyle;
	};
}
