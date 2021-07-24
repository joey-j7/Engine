#pragma once

#include "UIComponent.h"
#include "Engine/Objects/Worlds/Entities/Entity.h"

#include "include/utils/SkTextUtils.h"

namespace Engine
{
	class Engine_API UIText : public UIComponent
	{
	public:
		UIText(Entity& Entity, const std::string& Text = "", const std::string& sName = "Text");

		const std::string& GetText() const { return m_Text; };
		void SetText(const std::string& Text);

		uint32_t GetFontSize() const { return m_FontSize; };
		void SetFontSize(uint32_t Size);

		virtual void Draw() override;
		virtual void SetAntialiasing(bool AA) override;
		
	private:
		void MeasureSize();
		
		std::string m_Text = "";
		uint32_t m_FontSize = 64;

		SkFont m_Font;

		SkTextUtils::Align m_Alignment = SkTextUtils::Align::kLeft_Align;
	};

	Engine_REGISTER_COMP(UIText);
}
