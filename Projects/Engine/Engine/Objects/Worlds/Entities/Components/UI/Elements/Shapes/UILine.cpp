#include "pch.h"
#include "UILine.h"

#include "Platform/Window.h"

namespace Engine
{
	UILine::UILine(Entity& Entity, const String& sName) : UIShape(Entity, sName)
	{
		SetBorder(m_BorderWidth, m_Color, false);
	}
	
	void UILine::Draw()
	{
		const Vector2 End = GetEndPosition();

		m_Canvas->drawLine(
			 0.f,
			0.f,
			End.x,
			End.y,
			m_Paint
		);
	}

	void UILine::SetStartPosition(const Vector2& Start)
	{
		if (GetPosition() == Start)
			return;

		// Marks dirty
		SetPosition(Start);
		const Vector2 StartPos = GetStartPosition();

		m_Width = m_End.x - StartPos.x;
		m_Height = m_End.y - StartPos.y;
	}

	void UILine::SetEndPosition(const Vector2& End)
	{
		if (m_End == End)
			return;

		m_End = End;
		MarkDirty();

		const Vector2 Start = GetStartPosition();

		SetWidth(glm::max(Start.x, m_End.x) - glm::min(Start.x, m_End.x));
		SetHeight(glm::max(Start.y, m_End.y) - glm::min(Start.y, m_End.y));
	}
}