#pragma once

#include "Platform/Window.h"

// A clickable can be hovered, unhovered, pressed, released and clicked
// Implementation includes bound, visibility checking, focussability etc.
// Polls mouse input through events

namespace Engine
{
	class Engine_API Clickable
	{
	public:
		Clickable();
		virtual ~Clickable();

		virtual const AABB GetBounds() const = 0;

		void Click();

		bool IsHovered() const { return m_IsHovered; }
		bool IsPressed() const { return m_IsPressed; }

		Event<void, const DVector2&> OnEnterEvent = Event<void, const DVector2&>("Clickable::OnEnter");
		Event<void, const DVector2&> OnHoverEvent = Event<void, const DVector2&>("Clickable::OnHover");
		Event<void, const DVector2&> OnExitEvent = Event<void, const DVector2&>("Clickable::OnExit");
		Event<void, const DVector2&, const DVector2&> OnDraggedEvent = Event<void, const DVector2&, const DVector2&>("Clickable::OnDragged");
		Event<void, const DVector2&> OnScrolledEvent = Event<void, const DVector2&>("Clickable::OnScrolled");

		Event<void> OnPressedEvent = Event<void>("Clickable::OnPressed");
		Event<void> OnReleasedEvent = Event<void>("Clickable::OnReleased");
		Event<void> OnClickedEvent = Event<void>("Clickable::OnClicked");

		bool CanFocus() const { return IsAlwaysFocussable() || !FocussedClickable || FocussedClickable == this; }
		virtual bool IsClickable() const { return m_IsActive; }

		void SetAlwaysFocussable(bool Focussable) { m_AlwaysFocussable = Focussable; }
		bool IsAlwaysFocussable() const { return m_AlwaysFocussable; }

	protected:
		virtual void OnEnter(const DVector2& Position);
		virtual void OnHover(const DVector2& Position);
		virtual void OnExit(const DVector2& Position);
		virtual void OnDrag(const DVector2& Position, const DVector2& Delta);

		virtual bool OnPressed();
		virtual void OnReleased();
		virtual void OnClicked();

		bool m_AlwaysFocussable = false;

		bool m_IsHovered = false;
		bool m_IsPressed = false;

		bool m_IsActive = true;

		DVector2 m_PressPosition;

		uint32_t m_CursorPositionEventID = UINT_MAX;
		uint32_t m_MousePressedEventID = UINT_MAX;
		uint32_t m_MouseReleasedEventID = UINT_MAX;
		uint32_t m_ScrollEventID = UINT_MAX;

	protected:
		static Clickable* FocussedClickable;

		void OnCursorPosition(const DVector2& Position, const DVector2& Delta);
		void OnMousePressed(uint32_t MouseButton);
		void OnMouseReleased(uint32_t MouseButton);
		void OnMouseScroll(const DVector2& Delta);

		Window& m_Window;
	};
}
