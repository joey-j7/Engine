#include "pch.h"
#include "UIElement.h"

#include "Engine/Objects/Worlds/Entities/Components/UI/Layouts/UILayout.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Renderables/UIRenderable.h"

namespace Engine
{
	AABB UIElement::GetCanvas(Entity& Origin) const
	{
		Window& Window = Application::Get().GetRenderContext().GetWindow();
		AABB Bounds;

		// Get parent bounds, otherwise fall back to window size
		if (Entity* Parent = Origin.GetParent())
		{
			Bounds = Parent->GetBounds();
		}
		else
		{
			Bounds.fLeft = Bounds.fTop = 0.f;
			Bounds.fRight = Window.GetWidth();
			Bounds.fBottom = Window.GetHeight();
		}

		// Finding a layout with child element recursively
		const LayoutEntity Search = FindLayout(&Origin);

		// No offset here as Elements need to be a direct child of Layouts
		if (!Search.Entity || !Search.Layout)
			return Bounds;

		Vector2 Offset = Search.Layout->GetElement(*Search.Entity).Offset;
		Offset += Search.Layout->GetScrollPosition();

		// Make local
		Bounds.fRight -= Bounds.fLeft;
		Bounds.fLeft = 0.f;
		Bounds.fBottom -= Bounds.fTop;
		Bounds.fTop = 0.f;

		Bounds.fLeft += Offset.x;
		Bounds.fBottom += Offset.y;
		Bounds.fRight += Offset.x;
		Bounds.fTop += Offset.y;

		return Bounds;
	}

	UIElement::LayoutEntity UIElement::FindLayout(Entity* Search, Entity* Origin) const
	{
		if (!Search)
			return {};

		auto Layouts = Search->GetComponentsOfType<UILayout>();

		if (Layouts.empty())
		{
			// Nested loop
			if (Search->GetParent())
				return FindLayout(Search->GetParent(), Search);
		}

		LayoutEntity Result;

		Result.Layout = Layouts.empty() ? nullptr : Layouts[0];
		Result.Entity = Origin;

		return Result;
	}
}