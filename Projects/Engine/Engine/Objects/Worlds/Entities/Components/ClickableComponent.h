#pragma once

#include "Engine/Objects/Clickable.h"

namespace Engine
{
	class Engine_API ClickableComponent : public Component, public Clickable
	{
	public:
		ClickableComponent(Entity& Entity, const String& sName = "Clickable");
		~ClickableComponent();
		
		virtual const AABB GetBounds() const override;
		virtual bool IsClickable() const override;

	private:
		void OnVisibilityChanged(Entity::Visibility Vis);
		uint32_t m_VisibilityHandler = UINT_MAX;
	};
	
	Engine_REGISTER_COMP(ClickableComponent);
}
