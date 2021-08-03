#include "pch.h"
#include "UILine.h"

namespace Engine
{
	UILine::UILine(Entity& Entity, const String& sName) : UIShape(Entity, sName)
	{
		m_PaintStyle = SkPaint::kStroke_Style;
		SetEndPosition(GetStartPosition() + m_End);
	}
	
	void UILine::Draw()
	{
		const Vector2 Start = GetStartPosition();
		const Vector2 End = Start + GetEndPosition();
		
		m_Canvas->drawLine(
			Start.x,
			Start.y,
			End.x,
			End.y,
			m_Paint
		);
	}

	void UILine::SetEndPosition(const Vector2& End)
	{
		if (m_End == End)
			return;

		m_End = End;
		MarkDirty();
	}
}