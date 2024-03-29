#pragma once

#include "UIRenderable.h"
#include "include/utils/SkTextUtils.h"

namespace Engine
{
	class Engine_API UIText : public UIRenderable
	{
	public:
		UIText(Entity& Entity, const String& Text = "", const String& sName = "Text");

		const String& GetText() const { return m_Text; };
		void SetText(const String& Text);

		uint32_t GetFontSize() const { return m_FontSize; };
		void SetFontSize(uint32_t Size);

		virtual void Draw() override;
		virtual void SetAntialiasing(bool AA) override;
		
	private:
		void MeasureSize();
		
		String m_Text = "";
		uint32_t m_FontSize = 18;

		SkFont m_Font;

		uint32_t m_TextWidth = 0;
		uint32_t m_TextHeight = 0;
	};
}
