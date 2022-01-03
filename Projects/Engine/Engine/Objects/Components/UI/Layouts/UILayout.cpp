#include "pch.h"
#include "UILayout.h"

#include "Engine/Application.h"

#include "Engine/Objects/Components/Transform/Transform2DComponent.h"
#include "Engine/Objects/Components/UI/Renderables/UIRenderable.h"
#include "Engine/Objects/Components/ClickableComponent.h"
#include <glm/gtx/compatibility.hpp>

namespace Engine
{
	UILayout::UILayout(Entity& Entity, const String& sName) : Component(Entity, sName)
	{
		AddDependencyTypes<Transform2DComponent, ClickableComponent>();
		AddProhibitedTypes<UIRenderable>();

		m_ChildAddedID = m_Entity.OnChildAdded.Bind(this, &UILayout::OnChildAdded);
		m_ChildRemovedID = m_Entity.OnChildRemoved.Bind(this, &UILayout::OnChildRemoved);

		m_ParentChangedID = m_Entity.OnParentChanged.Bind(this, &UILayout::OnParentChanged);

		m_Transform = GetEntity().GetComponent<Transform2DComponent>();
		m_Clickable = GetEntity().GetComponent<ClickableComponent>();

		m_Clickable->SetAlwaysFocussable(true);

		m_Clickable->OnPressedEvent.Bind(this, &UILayout::OnPressed);
		m_Clickable->OnReleasedEvent.Bind(this, &UILayout::OnReleased);
		m_Clickable->OnDraggedEvent.Bind(this, &UILayout::OnDragged);
		m_Clickable->OnScrolledEvent.Bind(this, &UILayout::OnScrolled);
	}

	UILayout::~UILayout()
	{
		m_Entity.OnChildAdded.Unbind(m_ChildAddedID);
		m_Entity.OnChildRemoved.Unbind(m_ChildRemovedID);
		m_Entity.OnParentChanged.Unbind(m_ParentChangedID);

		GetWorld().OnUpdate.Unbind(m_BounceEventID);
	}

	AABB UILayout::GetBounds() const
	{
		AABB Canvas = GetCanvas(GetEntity());
		AABB Bounds = Canvas;

		Bounds.fLeft -= Canvas.fLeft;
		Bounds.fTop -= Canvas.fTop;
		Bounds.fRight -= Canvas.fLeft;
		Bounds.fBottom -= Canvas.fTop;
		
		// Scale
		Bounds.fLeft *= m_Transform->GetScale().x;
		Bounds.fTop *= m_Transform->GetScale().y;
		Bounds.fRight *= m_Transform->GetScale().x;
		Bounds.fBottom *= m_Transform->GetScale().y;

		// Add position
		Bounds.fLeft += m_Transform->GetPosition().x;
		Bounds.fTop += m_Transform->GetPosition().y;
		Bounds.fRight += m_Transform->GetPosition().x;
		Bounds.fBottom += m_Transform->GetPosition().y;

		// Add canvas offset
		Bounds.fLeft += Canvas.fLeft;
		Bounds.fTop += Canvas.fTop;
		Bounds.fRight += Canvas.fLeft;
		Bounds.fBottom += Canvas.fTop;

		return Bounds;
	}

	AABB UILayout::GetElementBounds() const
	{
		AABB Bounds;

		Bounds.fLeft = FLT_MAX;
		Bounds.fRight = -FLT_MAX;
		Bounds.fTop = FLT_MAX;
		Bounds.fBottom = -FLT_MAX;

		for (auto& Pair : m_ElementEntities)
		{
			AABB EntityBounds = Pair.first->GetBounds();

			Bounds.fLeft = glm::min(Bounds.fLeft, EntityBounds.fLeft);
			Bounds.fRight = glm::max(Bounds.fRight, EntityBounds.fRight);
			Bounds.fTop = glm::min(Bounds.fTop, EntityBounds.fTop);
			Bounds.fBottom = glm::max(Bounds.fBottom, EntityBounds.fBottom);
		}

		return Bounds;
	}

	void UILayout::SetScrollPosition(const Vector2& ScrollPosition)
	{
		if (m_ScrollPosition == ScrollPosition)
			return;

		Window& Window = Application::Get().GetRenderContext().GetWindow();
		const float Scale = Window.GetScale();

		Vector2 Offset = m_Transform->GetPosition() * Scale;

		Vector2 Scroll = ScrollPosition;

		AABB Bounds = GetBounds();
		AABB ElementBounds = GetElementBounds();

		float ScrollWidth = glm::max(0.f, ElementBounds.width() - Bounds.width() + Offset.x);
		float ScrollHeight = glm::max(0.f, ElementBounds.height() - Bounds.height() + Offset.y);

		Scroll.x = glm::min(0.f, Scroll.x);
		Scroll.x = glm::max(-ScrollWidth, Scroll.x);
		Scroll.y = glm::min(0.f, Scroll.y);
		Scroll.y = glm::max(-ScrollHeight, Scroll.y);

		if (m_ScrollPosition == Scroll)
			return;

		m_ScrollPosition = Scroll;
	}

	void UILayout::SetOverlap(bool Overlap)
	{
		if (m_OverlapElements == Overlap)
			return;

		m_OverlapElements = Overlap;

		Rebuild();
	}

	void UILayout::SetGridCount(uint32_t ColumnCount, uint32_t RowCount)
	{
		if (m_GridCount.x == ColumnCount && m_GridCount.y == RowCount)
			return;

		m_GridCount.x = ColumnCount;
		m_GridCount.y = RowCount;

		Rebuild();
	}

	void UILayout::AlignElementsToGrid(bool X, bool Y)
	{
		if (m_AlignElementsToGridX == X && m_AlignElementsToGridY == Y)
			return;

		m_AlignElementsToGridX = X;
		m_AlignElementsToGridY = Y;

		Rebuild();
	}

	void UILayout::SetUniformSpacing(bool Uniform)
	{
		if (m_UniformSpacing == Uniform)
			return;

		m_UniformSpacing = Uniform;
		m_Spacing = Vector2(0.f);

		Rebuild();
	}

	const UILayout::Element& UILayout::GetElement(Entity& Element)
	{
		Entity* Entity = &Element;
		auto Find = m_ElementEntities.find(Entity);

		if (Find != m_ElementEntities.end())
			return Find->second;

		return {};
	}

	void UILayout::Rebuild()
	{
		std::vector<Entity*> Entities;
		Entities.reserve(m_ElementEntities.size());

		uint32_t Count = 0;

		for (auto Child : m_ElementEntities)
		{
			Entities.push_back(Child.second.Entity);
			RemoveElement(*Child.second.Entity);

			Count++;
		}

		for (auto Entity : Entities)
		{
			UVector2 Index = UVector2(
				(m_GridCount.x == 0 ? 0 : (m_ElementEntities.size() % m_GridCount.x)) +
				(m_GridCount.y == 0 ? 0 : static_cast<uint32_t>(
					std::floor(m_ElementEntities.size() / m_GridCount.y)
				)),

				(m_GridCount.x == 0 ? 0 : static_cast<uint32_t>(
					std::floor(m_ElementEntities.size() / m_GridCount.x)
				)) +
				(m_GridCount.y == 0 ? 0 : (m_ElementEntities.size() % m_GridCount.y))
			);

			AddElement(Index, *Entity);
		}
	}

	void UILayout::OnChildAdded(Entity& Origin, Entity& Child)
	{
		UVector2 Index = UVector2(
			(m_GridCount.x == 0 ? 0 : (m_ElementEntities.size() % m_GridCount.x)) +
			(m_GridCount.y == 0 ? 0 : static_cast<uint32_t>(
				std::floor(m_ElementEntities.size() / m_GridCount.y)
			)),

			(m_GridCount.x == 0 ? 0 : static_cast<uint32_t>(
				std::floor(m_ElementEntities.size() / m_GridCount.x)
			)) +
			(m_GridCount.y == 0 ? 0 : (m_ElementEntities.size() % m_GridCount.y))
		);

		AddElement(Index, Child);
	}

	void UILayout::OnChildRemoved(Entity& Origin, Entity& Child)
	{
		RemoveElement(Child);
	}

	void UILayout::OnParentChanged(Entity& Origin, Entity* Old, Entity* New)
	{
		Rebuild();
	}
	
	Vector2 UILayout::CalculateOffset(const UVector2& Index)
	{
		if (m_OverlapElements)
			return Vector2(0.f);

		Window& Window = Application::Get().GetRenderContext().GetWindow();
		const float Scale = Window.GetScale();

		Vector2 Offset = m_Transform->GetPosition() * Scale;
		AABB Bounds = GetBounds();

		// Fit to bounds
		if (m_AlignElementsToGridX)
		{
			Offset.x = (Bounds.width()) / glm::max((uint32_t)1, m_GridCount.x) * Index.x;
		}
		// Fit to previous element
		else if (Index.x > 0)
		{
			UILayout::Element* PrevElement = nullptr;
			AABB PrevBounds;

			if (m_GridCount.y > 0)
			{
				PrevElement = m_ElementIndex[Index.x - 1][0];
				PrevBounds = PrevElement->Entity->GetBounds();

				for (uint32_t i = 1; i < m_GridCount.y; ++i)
				{
					PrevElement = m_ElementIndex[Index.x - 1][i];

					if (!PrevElement)
						continue;

					AABB Bounds = PrevElement->Entity->GetBounds();

					PrevBounds.fTop = glm::min(PrevBounds.fTop, Bounds.fTop);
					PrevBounds.fBottom = glm::max(PrevBounds.fBottom, Bounds.fBottom);
					PrevBounds.fLeft = glm::min(PrevBounds.fLeft, Bounds.fLeft);
					PrevBounds.fRight = glm::max(PrevBounds.fRight, Bounds.fRight);
				}
			}
			else
			{
				PrevElement = m_ElementIndex[Index.x - 1][Index.y];
				PrevBounds = PrevElement->Entity->GetBounds();
			}

			Offset.x = PrevElement->Offset.x + glm::floor(PrevBounds.width());
			Offset.x = std::max(m_Spacing.x, Offset.x);
		}

		// Fit to bounds
		if (m_AlignElementsToGridY)
		{
			Offset.y = (Bounds.height()) / glm::max((uint32_t)1, m_GridCount.y) * Index.y;
		}
		// Fit to previous element
		else if (Index.y > 0)
		{
			UILayout::Element* PrevElement = nullptr;
			AABB PrevBounds;

			if (m_GridCount.x > 0)
			{
				PrevElement = m_ElementIndex[0][Index.y - 1];
				PrevBounds = PrevElement->Entity->GetBounds();

				for (uint32_t i = 1; i < m_GridCount.x; ++i)
				{
					PrevElement = m_ElementIndex[i][Index.y - 1];

					if (!PrevElement)
						continue;

					AABB Bounds = PrevElement->Entity->GetBounds();
					
					PrevBounds.fTop = glm::min(PrevBounds.fTop, Bounds.fTop);
					PrevBounds.fBottom = glm::max(PrevBounds.fBottom, Bounds.fBottom);
					PrevBounds.fLeft = glm::min(PrevBounds.fLeft, Bounds.fLeft);
					PrevBounds.fRight = glm::max(PrevBounds.fRight, Bounds.fRight);
				}
			}
			else
			{
				PrevElement = m_ElementIndex[Index.x][Index.y - 1];
				PrevBounds = PrevElement->Entity->GetBounds();
			}

			Offset.y = PrevElement->Offset.y + glm::floor(PrevBounds.height());
			Offset.y = std::max(m_Spacing.y, Offset.y);
		}
		
		return Offset;
	}
	
	void UILayout::AddElement(const UVector2& Index, Entity& Entity)
	{
		Element Data;

		Data.Index = Index;
		Data.Offset = CalculateOffset(Data.Index);
		Data.Entity = &Entity;

		auto Find = m_ElementEntities.find(&Entity);

		// Remove the element first so we're clean
		if (Find != m_ElementEntities.end())
		{
			RemoveElement(Entity);
		}
		// Since RemoveElement does this too, only execute if not removed
		else
		{
			// Component layout will be modified; requires recalculation through MarkDirty()
			auto Components = Entity.GetComponentsOfType<UIRenderable>();

			for (auto Component : Components)
			{
				Component->MarkDirty();
			}
		}

		// Add to children
		m_ElementEntities.emplace(&Entity, Data);

		// Add to indexed list
		auto Find2 = m_ElementIndex.find(Data.Index.x);

		if (Find2 == m_ElementIndex.end())
		{
			m_ElementIndex.emplace(Data.Index.x, std::map<uint32_t, Element*>());
			Find2 = m_ElementIndex.find(Data.Index.x);
		}

		Find2->second.emplace(Data.Index.y, &m_ElementEntities[&Entity]);

		// Uniform spacing
		m_Spacing = Vector2(0.f);

		if (m_UniformSpacing)
		{
			for (auto& Pair : m_ElementEntities)
			{
				AABB Bounds = Pair.second.Entity->GetBounds();

				m_Spacing.x = std::max(m_Spacing.x, Bounds.fRight - Bounds.fLeft);
				m_Spacing.y = std::max(m_Spacing.y, Bounds.fBottom - Bounds.fTop);
			}
		}
	}

	void UILayout::RemoveElement(Entity& Entity)
	{
		UVector2 Index(0.f);
		auto Find = m_ElementEntities.find(&Entity);

		// Not found
		if (Find == m_ElementEntities.end())
			return;

		// Component layout will be reset; requires recalculation through MarkDirty()
		auto Components = Entity.GetComponentsOfType<UIRenderable>();

		for (auto Component : Components)
		{
			Component->MarkDirty();
		}

		// Remove from two lists
		Index = Find->second.Index;
		m_ElementEntities.erase(Find);
		
		m_ElementIndex[Index.x].erase(Index.y);

		if (m_ElementIndex[Index.x].empty())
			m_ElementIndex.erase(Index.x);

		// Uniform spacing
		m_Spacing = Vector2(0.f);

		if (m_UniformSpacing)
		{
			for (auto& Pair : m_ElementEntities)
			{
				AABB Bounds = Pair.second.Entity->GetBounds();

				m_Spacing.x = std::max(m_Spacing.x, Bounds.fRight - Bounds.fLeft);
				m_Spacing.y = std::max(m_Spacing.y, Bounds.fBottom - Bounds.fTop);
			}
		}
	}

	void UILayout::OnPressed()
	{
		GetWorld().OnUpdate.Unbind(m_BounceEventID);
		m_BounceEventID = UINT_MAX;

		m_ScrollDelta = DVector2(0.0);
	}

	void UILayout::OnReleased()
	{
		double DeltaLength = m_ScrollDelta.Length();

		if (DeltaLength == 0.0 || m_ScrollSpeed <= 0.f)
			return;
		
		Window& Window = Application::Get().GetRenderContext().GetWindow();
		double Max = glm::max(Window.GetWidth(), Window.GetHeight()) * 0.25;

		DVector2 Delta;
		Delta.x = glm::min(Max, m_ScrollDelta.x);
		Delta.y = glm::min(Max, m_ScrollDelta.y);

		DeltaLength = Delta.Length();

		GetWorld().OnUpdate.Unbind(m_BounceEventID);
		m_BounceEventID = GetWorld().OnUpdate.Bind(this, &UILayout::OnBounce);
	}
	
	void UILayout::OnDragged(const DVector2& Position, const DVector2& Delta)
	{
		if (!m_CanScroll)
			return;

		if (Delta.Length() != 0.0)
		{
			m_ScrollDelta = Delta;
			// m_ScrollDelta.x = glm::lerp(m_ScrollDelta.x, Delta.x, 0.33);
			// m_ScrollDelta.y = glm::lerp(m_ScrollDelta.y, Delta.y, 0.33);
		}

		Scroll(m_ScrollDelta);
	}

	void UILayout::OnScrolled(const DVector2& Delta)
	{
		if (!m_CanScroll)
			return;

		if (Delta.Length() != 0.0)
		{
			m_ScrollDelta = Delta * 20.f;
		}

		Scroll(m_ScrollDelta);
	}
	
	void UILayout::OnBounce(float DeltaTime)
	{
		double Length = m_ScrollDelta.Length();
		
		if (!m_Clickable->IsPressed() && Length >= 0.1f)
		{
			m_ScrollDelta = glm::lerp(
				glm::vec2(0.f),
				glm::vec2(m_ScrollDelta.x, m_ScrollDelta.y),
				0.98f
			);

			Length = m_ScrollDelta.Length();
			Scroll(m_ScrollDelta * m_ScrollSpeed);

			return;
		}

		GetWorld().OnUpdate.Unbind(m_BounceEventID);
		m_BounceEventID = UINT_MAX;
	}
	
	void UILayout::Scroll(const DVector2& Delta)
	{
		SetScrollPosition(m_ScrollPosition + Delta);
	}
}
