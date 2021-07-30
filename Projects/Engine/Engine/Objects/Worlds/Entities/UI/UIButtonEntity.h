#pragma once

#include "Engine/Objects/Worlds/Entities/StaticEntity.h"

#include "Engine/Objects/Worlds/Entities/Components/ClickableComponent.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Elements/Shapes/UIRect.h"

#include "Engine/Objects/Worlds/Entities/Components/UI/Elements/UIText.h"

namespace Engine
{
	class Engine_API UIButtonEntity : public StaticEntity
	{
	public:
		UIButtonEntity(
			const std::string& Text,
			std::function<void()> Function,
			const std::string& sName = "Button Entity"
		);

		void SetText(const std::string& Text);

	protected:
		virtual void OnEnter(const DVector2& Position);
		virtual void OnExit(const DVector2& Position);

		virtual void OnPressed();
		virtual void OnReleased();

		virtual void OnClicked();

		std::function<void()> m_Function;
		
		ClickableComponent* m_ClickableComponent = nullptr;
		UIRect* m_RectangleComponent = nullptr;
		UIText* m_TextComponent = nullptr;
	};
}
