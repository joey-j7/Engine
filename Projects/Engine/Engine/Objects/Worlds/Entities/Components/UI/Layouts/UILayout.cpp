#include "pch.h"
#include "UILayout.h"

#include "Engine/Application.h"

namespace Engine
{
	UILayout::UILayout(Entity& Entity, const String& sName) : Component(Entity, sName)
	{
		AddDependencyTypes<Transform2DComponent>();
		AddProhibitedTypes<UIElement>();

		m_ChildAddedID = m_Entity.OnChildAdded.Bind(this, &UILayout::OnChildAdded);
		m_ChildRemovedID = m_Entity.OnChildRemoved.Bind(this, &UILayout::OnChildRemoved);

		m_ParentChangedID = m_Entity.OnParentChanged.Bind(this, &UILayout::OnParentChanged);

		m_Transform = GetEntity().GetComponent<Transform2DComponent>();
	}

	UILayout::~UILayout()
	{
		m_Entity.OnChildAdded.Unbind(m_ChildAddedID);
		m_Entity.OnChildRemoved.Unbind(m_ChildRemovedID);

		m_Entity.OnParentChanged.Unbind(m_ParentChangedID);
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
			
			Bounds.fLeft = Bounds.fTop = 0.f;
			Bounds.fRight = Window.GetWidth();
			Bounds.fBottom = Window.GetHeight();
		}
		
		Bounds.fLeft *= m_Transform->GetScale().x;
		Bounds.fTop *= m_Transform->GetScale().y;
		Bounds.fRight *= m_Transform->GetScale().x;
		Bounds.fBottom *= m_Transform->GetScale().y;

		Bounds.fLeft += m_Transform->GetPosition().x;
		Bounds.fTop += m_Transform->GetPosition().y;
		Bounds.fRight += m_Transform->GetPosition().x;
		Bounds.fBottom += m_Transform->GetPosition().y;

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

		// Child.OnComponentAdded.Bind(this, &UILayout::OnComponentAdded);
		// Child.OnComponentRemoved.Bind(this, &UILayout::OnComponentRemoved);
	}

	void UILayout::OnChildRemoved(Entity& Origin, Entity& Child)
	{
		auto Find = m_EntitiesComponents.find(&Child);
		
		for (auto& Pair : Find->second)
		{
			// Pair.Component->SetLayoutOffset(Vector2(0.f));
		}

		// Child.OnComponentAdded.Unbind(this, &UILayout::OnComponentAdded);
		// Child.OnComponentRemoved.Unbind(this, &UILayout::OnComponentRemoved);

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

		// UI->SetLayoutOffset(Vector2(0.f));
		
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
