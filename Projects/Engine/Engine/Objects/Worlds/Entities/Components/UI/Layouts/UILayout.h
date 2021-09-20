#pragma once

#include "Engine/Objects/Worlds/Entities/EntityComponent.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/UIElement.h"

namespace Engine
{
	class ClickableComponent;

	class Engine_API UILayout : public Component, public UIElement
	{
	public:
		struct Element
		{
			Vector2 Offset = Vector2(0.f);
			UVector2 Index = UVector2(0);

			Entity* Entity = nullptr;
		};
		
		UILayout(Entity& Entity, const String& sName = "UI Layout");
		virtual ~UILayout();

		virtual AABB GetBounds() const;
		virtual AABB GetElementBounds() const;

		bool DoesOverlap() const { return m_OverlapElements; }
		void SetOverlap(bool Overlap);

		bool CanScroll() const { return m_CanScroll; }
		void SetScroll(bool Scroll) { m_CanScroll = Scroll; }

		const Vector2& GetScrollPosition() const { return m_ScrollPosition; }

		void SetElementCount(uint32_t Count, bool IsX = true);
		void SetFitElements(bool X, bool Y);
		void SetUniformSpacing(bool Uniform);

		const Element& GetElement(Entity& Entity);
		
		void Rebuild();

	protected:
		Transform2DComponent* m_Transform = nullptr;

	private:
		void OnChildAdded(Entity& Origin, Entity& Entity);
		void OnChildRemoved(Entity& Origin, Entity& Entity);
		
		void OnParentChanged(Entity& Origin, Entity* Old, Entity* New);
		
		Vector2 CalculateOffset(const UVector2& Index);

		void AddElement(const UVector2& Index, Entity& Entity);
		void RemoveElement(Entity& Entity);

		void OnPressed();
		void OnReleased();
		void OnDragged(const DVector2& Position, const DVector2& Delta);
		void OnScrolled(const DVector2& Delta);
		void OnBounce(float DeltaTime);

		void Scroll(const DVector2& Delta);

		// 0 = infinite
		UVector2 m_ElementCount = UVector2(1, 0);

		// Make items stack on top of each other
		bool m_OverlapElements = false;
		bool m_CanScroll = true;

		ClickableComponent* m_Clickable = nullptr;
		
		Vector2 m_ScrollPosition = Vector2(0.f);
		DVector2 m_ScrollDelta = DVector2(0.f);

		uint32_t m_BounceEventID = UINT_MAX;
		float m_ScrollSpeed = 1.5f;

		// Make the items fit across the whole layout
		bool m_FitElementsX = false;
		bool m_FitElementsY = false;

		// Makes sure every item takes up the same amount of space
		bool m_UniformSpacing = false;
		Vector2 m_Spacing = Vector2(0.01f);

		Event<void, Entity&, Entity&>::HandlerType::IDType m_ChildAddedID;
		Event<void, Entity&, Entity&>::HandlerType::IDType m_ChildRemovedID;
		Event<void, Entity&, Entity*, Entity*>::HandlerType::IDType m_ParentChangedID;

		std::unordered_map<Entity*, Element> m_ElementEntities;
		std::map<uint32_t, std::map<uint32_t, Element*>> m_ElementIndex;
	};
}
