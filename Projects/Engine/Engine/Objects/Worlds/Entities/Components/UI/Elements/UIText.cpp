#include "pch.h"
#include "UIText.h"

#include <include/core/SkFont.h>
#include <include/core/SkTextBlob.h>

namespace Engine
{
	UIText::UIText(Entity& Entity, const std::string& Text, const std::string& sName) : UIElement(Entity, sName)
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
		if (m_Text.empty())
			return;

		m_Canvas->drawTextBlob(
			SkTextBlob::MakeFromText(
				m_Text.c_str(),
				m_Text.size(),
				m_Font,
				SkTextEncoding::kUTF8
			),
			(m_Width - m_TextWidth) * m_Alignment.x,
			(m_Height - m_TextHeight) * m_Alignment.y + m_TextHeight,
			m_Paint
		);
	}

	void UIText::SetAntialiasing(bool AA)
	{
		UIElement::SetAntialiasing(AA);
		m_Font.setSubpixel(AA);
	}

	void UIText::MeasureSize()
	{
		AABB Bounds;
		
		SetWidth(m_Font.measureText(m_Text.c_str(), m_Text.size(), SkTextEncoding::kUTF8, &Bounds));
		SetHeight(Bounds.fBottom - Bounds.fTop);

		m_TextWidth = m_Width;
		m_TextHeight = m_Height;
	}
}
