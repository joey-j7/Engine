#pragma once

#include "Platform/Window.h"

namespace Engine
{
	class Engine_API Clickable
	{
	public:
		Clickable();
		virtual ~Clickable();

		virtual const AABB GetBounds() const = 0;

		bool IsHovered() const { return m_IsHovered; }
		bool IsPressed() const { return m_IsPressed; }

		Event<void, const DVector2&> OnEnterEvent = Event<void, const DVector2&>("Clickable::OnEnter");
		Event<void, const DVector2&> OnHoverEvent = Event<void, const DVector2&>("Clickable::OnHover");
		Event<void, const DVector2&> OnExitEvent = Event<void, const DVector2&>("Clickable::OnExit");
		Event<void, const DVector2&, const DVector2&> OnDraggedEvent = Event<void, const DVector2&, const DVector2&>("Clickable::OnDragged");

		Event<void> OnPressedEvent = Event<void>("Clickable::OnPressed");
		Event<void> OnReleasedEvent = Event<void>("Clickable::OnReleased");
		Event<void> OnClickedEvent = Event<void>("Clickable::OnClicked");

		bool CanFocus() const { return !FocussedClickable || FocussedClickable == this; }
		
	protected:
		virtual void OnEnter(const DVector2& Position);
		virtual void OnHover(const DVector2& Position);
		virtual void OnExit(const DVector2& Position);
		virtual void OnDrag(const DVector2& Position, const DVector2& Delta);

		virtual bool OnPressed();
		virtual void OnReleased();
		virtual void OnClicked();

		bool m_IsHovered = false;
		bool m_IsPressed = false;

	protected:
		static Clickable* FocussedClickable;

	private:
		void OnCursorPosition(const DVector2& Position, const DVector2& Delta);
		void OnMousePressed(uint32_t MouseButton);
		void OnMouseReleased(uint32_t MouseButton);

		Window& m_Window;
	};
}
