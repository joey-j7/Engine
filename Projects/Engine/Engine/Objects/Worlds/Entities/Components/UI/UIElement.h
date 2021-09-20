#pragma once

namespace Engine
{
	class Entity;
	class Component;

	class UILayout;
	class UIRenderable;

	class Engine_API UIElement
	{
	protected:
		struct LayoutEntity
		{
			UILayout* Layout = nullptr;
			Entity* Entity = nullptr;
		};

		AABB GetCanvas(Entity& Origin) const;
		LayoutEntity FindLayout(Entity* Search, Entity* Origin = nullptr) const;
	};
}