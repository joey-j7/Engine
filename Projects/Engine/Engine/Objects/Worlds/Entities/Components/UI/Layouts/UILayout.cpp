#include "pch.h"
#include "UILayout.h"

#include "Engine/Application.h"

namespace Engine
{
	UILayout::UILayout(Entity& Entity, const std::string& sName) : Component(Entity, sName)
	{
		AddDependencyTypes<TransformComponent2D>();
		AddProhibitedTypes<UIElement>();

		m_Entity.OnChildAdded.Bind(this, &UILayout::OnChildAdded);
		m_Entity.OnChildRemoved.Bind(this, &UILayout::OnChildRemoved);

		m_Entity.OnParentChanged.Bind(this, &UILayout::OnParentChanged);

		m_Transform = GetEntity().GetComponent<TransformComponent2D>();
	}

	AABB UILayout::GetBounds() const
	{
		AABB Bounds;
		
		// Nested loop
		if (const UILayout* Layout = CheckParentLayout(GetEntity()))
		{
			Bounds = Layout->GetBounds();
		}
		else
		{
			Window& Window = Application::Get().GetRenderContext().GetWindow();
			
			Bounds.lowerBound = Vector2(0.f);
			Bounds.upperBound = Vector2(Window.GetWidth(), Window.GetHeight());
		}
		
		Bounds.lowerBound.x *= m_Transform->GetScale().x;
		Bounds.lowerBound.y *= m_Transform->GetScale().y;
		Bounds.upperBound.x *= m_Transform->GetScale().x;
		Bounds.lowerBound.y *= m_Transform->GetScale().y;
		
		Bounds.lowerBound += m_Transform->GetPosition();
		Bounds.upperBound += m_Transform->GetPosition();

		return Bounds;
	}

	void UILayout::OnChildAdded(Entity& Origin, Entity& Child)
	{
		auto Components = Child.GetComponentsOfType<UIElement>();

		m_EntitiesComponents.insert(
			std::pair<Entity*, std::vector<ComponentData>>(
				&Child, std::vector<ComponentData>()
			)
		);

		std::vector<ComponentData>& Data = m_EntitiesComponents.at(&Child);
		Data.reserve(Components.size());
		
		for (UIElement* Component : Components)
		{
			ComponentData CompData(Component);
			CompData.IndexY = static_cast<uint32_t>(Data.size());
			
			Data.push_back(CompData);
			
			OnComponentMeasurement(
				Child,
				Data.back(), 
				static_cast<uint32_t>(Data.size()) - 1
			);
		}

		Child.OnComponentAdded.Bind(this, &UILayout::OnComponentAdded);
		Child.OnComponentRemoved.Bind(this, &UILayout::OnComponentRemoved);
	}

	void UILayout::OnChildRemoved(Entity& Origin, Entity& Child)
	{
		auto Find = m_EntitiesComponents.find(&Child);
		
		for (auto& Pair : Find->second)
		{
			Pair.Component->SetLayoutOffset(Vector2(0.f));
		}

		Child.OnComponentAdded.Unbind(this, &UILayout::OnComponentAdded);
		Child.OnComponentRemoved.Unbind(this, &UILayout::OnComponentRemoved);

		m_EntitiesComponents.erase(Find);
	}

	void UILayout::OnComponentAdded(Entity& Origin, Component& Component)
	{
		UIElement* UI = dynamic_cast<UIElement*>(&Component);

		if (!UI)
			return;

		m_EntitiesComponents.at(&Origin).push_back(UI);
		OnComponentMeasurement(
			Origin,
			m_EntitiesComponents.at(&Origin).back(),
			static_cast<uint32_t>(m_EntitiesComponents.at(&Origin).size()) - 1
		);
	}

	void UILayout::OnComponentRemoved(Entity& Origin, Component& Component)
	{
		UIElement* UI = dynamic_cast<UIElement*>(&Component);

		if (!UI)
			return;

		UI->SetLayoutOffset(Vector2(0.f));
		
		std::vector<ComponentData>& Components = m_EntitiesComponents.at(&Origin);
		Components.erase(std::find(Components.begin(), Components.end(), UI));
	}

	void UILayout::OnParentChanged(Entity& Origin, Entity* Old, Entity* New)
	{
		// NONE
	}

	UILayout* UILayout::CheckParentLayout(Entity& Origin) const
	{
		// Check if there's a layout in one of the parents
		auto Components = Origin.GetComponentsOfType<UILayout>();

		if (Components.empty())
		{
			// Nested loop
			if (Origin.GetParent())
				return CheckParentLayout(*Origin.GetParent());

			return nullptr;
		}

		return Components[0];
	}
}
