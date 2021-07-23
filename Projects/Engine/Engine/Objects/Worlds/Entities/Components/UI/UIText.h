#pragma once

#include "UIComponent.h"

#include "include/utils/SkTextUtils.h"

namespace Engine
{
	class Engine_API UIText : public UIComponent
	{
	public:
		UIText(Entity& Entity, const std::string& Text = "", const std::string& sName = "Text") : UIComponent(Entity, sName)
		{
			SetText(Text);
		}

		const std::string& GetText() const { return m_Text; };
		void SetText(const std::string& Text);

		uint32_t GetFontSize() const { return m_FontSize; };
		void SetFontSize(uint32_t Size);

		virtual void Draw() override;
		
	private:
		std::string m_Text = "";
		uint32_t m_FontSize = 64;

		SkTextUtils::Align m_Alignment = SkTextUtils::Align::kLeft_Align;
	};

	Engine_REGISTER_COMP(UIText);
}
