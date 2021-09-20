#pragma once

#include "UIShape.h"

namespace Engine
{
	class Engine_API UIRect : public UIShape
	{
	public:
		enum Type
		{
			E_DEFAULT = 0,
			E_ROUNDED = 1
		};
		
		UIRect(Entity& Entity, const String& sName = "Rect") : UIShape(Entity, sName)
		{

		}

		void Draw() override;
		
		Type GetType() const { return m_Type; }

		void SetRadiuses(SkScalar Left, SkScalar Top, SkScalar Right, SkScalar Bottom);
		void SetRadius(SkScalar Radius);
		
	private:
		Type m_Type = E_DEFAULT;
		SkScalar m_LeftRadius = 0.f,
			m_TopRadius = 0.f,
			m_RightRadius = 0.f,
			m_BottomRadius = 0.f;
	};

	Engine_REGISTER_COMP(UIRect);
}
