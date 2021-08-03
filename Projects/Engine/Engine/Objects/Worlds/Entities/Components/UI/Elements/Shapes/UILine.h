#pragma once

#include "UIShape.h"

namespace Engine
{
	class Engine_API UILine : public UIShape
	{
	public:
		UILine(Entity& Entity, const String& sName = "Line");

		void Draw() override;

		const Vector2& GetStartPosition() const { return GetPosition(); }
		void SetStartPosition(const Vector2& End) { SetPosition(End); };
		
		const Vector2& GetEndPosition() const { return m_End; }
		void SetEndPosition(const Vector2& End);

		uint32_t GetThickness() const { return m_BorderWidth; }
		void SetThickness(uint32_t Thickness) { SetBorderWidth(Thickness); };

	private:
		using UIElement::SetWidth;
		using UIElement::SetHeight;
		using UIElement::SetSize;

		using UIElement::GetBorderWidth;
		using UIElement::SetBorderWidth;
		
		Vector2 m_End = Vector2(30.f, 0.f);
	};

	Engine_REGISTER_COMP(UIRect);
}
