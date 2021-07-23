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
		
		UIRect(Entity& Entity, const std::string& sName = "Text") : UIShape(Entity, sName)
		{

		}

		virtual void Draw() override;
		
		Type GetType() const { return m_Type; }

		void SetRadiuses(SkScalar Left, SkScalar Top, SkScalar Right, SkScalar Bottom);
		void SetRadius(SkScalar Radius);
		
	private:
		Type m_Type = E_DEFAULT;
		SkScalar m_LeftRadius, m_TopRadius, m_RightRadius, m_BottomRadius;
	};

	Engine_REGISTER_COMP(UIRect);
}