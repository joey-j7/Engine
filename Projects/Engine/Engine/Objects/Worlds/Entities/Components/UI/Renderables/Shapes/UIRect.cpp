#include "pch.h"
#include "UIRect.h"

#include <include/core/SkRRect.h>
#include <include/core/SkTileMode.h>

namespace Engine
{
	void UIRect::Draw()
	{
		SkRect Rect = SkRect::MakeXYWH(
			0, 0,
			m_Bounds.width(),
			m_Bounds.height()
		);
		
		if (m_Type == E_ROUNDED)
		{
			SkRRect RRect;
			RRect.setNinePatch(Rect, m_LeftRadius, m_TopRadius, m_RightRadius, m_BottomRadius);

			m_Canvas->drawRRect(RRect, m_Paint);
		}
		else
		{
			m_Canvas->drawRect(Rect, m_Paint);
		}
	}

	void UIRect::SetRadiuses(SkScalar Left, SkScalar Top, SkScalar Right, SkScalar Bottom)
	{
		if (m_LeftRadius == Left && m_TopRadius == Top && m_RightRadius == Right && m_BottomRadius == Bottom)
			return;

		m_Type = Left == 0.f &&
			Top == 0.f &&
			Right == 0.f &&
			Bottom == 0.f ?
			E_DEFAULT :
			E_ROUNDED
		;

		m_LeftRadius = Left;
		m_TopRadius = Top;
		m_RightRadius = Right;
		m_BottomRadius = Bottom;
		
		MarkDirty();
	}

	void UIRect::SetRadius(SkScalar Radius)
	{
		SetRadiuses(Radius, Radius, Radius, Radius);
	}
}
