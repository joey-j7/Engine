#pragma once

#include "UIRect.h"

namespace Engine
{
	class Engine_API UIOval : public UIRect
	{
	public:
		UIOval(Entity& Entity, const String& sName = "Oval") : UIRect(Entity, sName)
		{

		}
		
		virtual void SetWidth(uint32_t Width) override;
		virtual void SetHeight(uint32_t Height) override;
		
	private:
		Type m_Type = E_DEFAULT;
		SkScalar m_LeftRadius = 0.f,
			m_TopRadius = 0.f,
			m_RightRadius = 0.f,
			m_BottomRadius = 0.f;
	};
}
