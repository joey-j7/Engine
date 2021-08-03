#pragma once

#include "Engine/Objects/Worlds/Entities/StaticEntity.h"

#include "Engine/Objects/Worlds/Entities/Components/ClickableComponent.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Elements/UIText.h"

namespace Engine
{
	class UIRect;

	class Engine_API UIButton : public StaticEntity
	{
	public:
		struct Style
		{
			String m_Text = "Test";
			
			Color m_Color = SK_ColorWHITE;
			UIElement::Gradient m_Gradient;
			
			String m_BackgroundImagePath = "";
			String m_ForegroundImagePath = "";

			float m_Radius = 20.f;
			Vector4 m_Padding = Vector4(20.f);
		};
		
		UIButton(
			std::function<void()> Function,
			const Style& DefaultStyle = Style(),
			const Style& HoverStyle = Style(),
			const Style& PressStyle = Style(),
			const String& sName = "Button"
		);

	protected:
		virtual void OnEnter(const DVector2& Position);
		virtual void OnExit(const DVector2& Position);

		virtual void OnPressed();
		virtual void OnReleased();

		virtual void OnClicked();

		void SetText(const String& Text);
		void ApplyStyle(const Style& NewStyle);

		std::function<void()> m_Function;
		
		ClickableComponent* m_ClickableComponent = nullptr;
		UIRect* m_BackgroundComponent = nullptr;
		UIImage* m_BackgroundImageComponent = nullptr;
		UIImage* m_ForegroundImageComponent = nullptr;
		UIText* m_TextComponent = nullptr;

		std::unique_ptr<StaticEntity> m_ForegroundEntity = nullptr;

		Style m_DefaultStyle;
		Style m_HoverStyle;
		Style m_PressStyle;
	};
}
