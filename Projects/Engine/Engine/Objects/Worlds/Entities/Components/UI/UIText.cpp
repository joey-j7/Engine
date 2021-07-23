#include "pch.h"
#include "UIText.h"

#include <include/core/SkFont.h>
#include <include/core/SkPaint.h>
#include <include/core/SkTextBlob.h>

namespace Engine
{
	void UIText::SetText(const std::string& Text)
	{
		if (m_Text == Text)
			return;
		
		m_Text = Text;
		MarkDirty();
	}

	void UIText::SetFontSize(uint32_t Size)
	{
		if (m_FontSize == Size)
			return;

		m_FontSize = Size;
		MarkDirty();
	}

	void UIText::Draw()
	{
		SkFont Font;
		Font.setSubpixel(true);
		Font.setSize(static_cast<SkScalar>(m_FontSize));
		
		SkRect Bounds;
		SkScalar Width = Font.measureText(m_Text.c_str(), m_Text.size(), SkTextEncoding::kUTF8, &Bounds);

		Vector2 Offset(0.f);
		
		if (m_Alignment != SkTextUtils::kLeft_Align) {
			if (m_Alignment == SkTextUtils::kCenter_Align) {
				Width *= 0.5f;
			}
			
			Offset.x -= Width;
		}

		Offset.y = Bounds.fBottom - Bounds.fTop;
		
		m_Canvas->drawTextBlob(
			SkTextBlob::MakeFromText(
				m_Text.c_str(),
				m_Text.size(),
				Font,
				SkTextEncoding::kUTF8
			),
			Offset.x,
			Offset.y,
			m_Paint
		);
	}
}
