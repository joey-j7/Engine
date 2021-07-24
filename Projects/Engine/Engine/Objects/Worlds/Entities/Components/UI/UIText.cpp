#include "pch.h"
#include "UIText.h"

#include <include/core/SkFont.h>
#include <include/core/SkPaint.h>
#include <include/core/SkTextBlob.h>

namespace Engine
{
	UIText::UIText(Entity& Entity, const std::string& Text, const std::string& sName) : UIComponent(Entity, sName)
	{
		m_Font.setSize(static_cast<SkScalar>(m_FontSize));
		m_Font.setSubpixel(m_UseAntialiasing);
		
		SetText(Text);
		MeasureSize();
	}

	void UIText::SetText(const std::string& Text)
	{
		if (m_Text == Text)
			return;
		
		m_Text = Text;
		MeasureSize();
		
		MarkDirty();
	}

	void UIText::SetFontSize(uint32_t Size)
	{
		if (m_FontSize == Size)
			return;

		m_FontSize = Size;
		m_Font.setSize(static_cast<SkScalar>(m_FontSize));
		
		MeasureSize();
		
		MarkDirty();
	}

	void UIText::Draw()
	{	
		SkScalar Width = static_cast<SkScalar>(m_Width);
		const SkScalar Height = static_cast<SkScalar>(m_Height);

		Vector2 Offset(0.f);
		
		if (m_Alignment != SkTextUtils::kLeft_Align) {
			if (m_Alignment == SkTextUtils::kCenter_Align) {
				Width *= 0.5f;
			}
			
			Offset.x -= Width;
		}

		Offset.y = Height;
		
		m_Canvas->drawTextBlob(
			SkTextBlob::MakeFromText(
				m_Text.c_str(),
				m_Text.size(),
				m_Font,
				SkTextEncoding::kUTF8
			),
			Offset.x,
			Offset.y,
			m_Paint
		);
	}

	void UIText::SetAntialiasing(bool AA)
	{
		UIComponent::SetAntialiasing(AA);
		m_Font.setSubpixel(AA);
	}

	void UIText::MeasureSize()
	{
		SkRect Bounds;
		
		SetWidth(m_Font.measureText(m_Text.c_str(), m_Text.size(), SkTextEncoding::kUTF8, &Bounds));
		SetHeight(Bounds.fBottom - Bounds.fTop);
	}
}
