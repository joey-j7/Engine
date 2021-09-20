#include "pch.h"
#include "Clickable.h"

#include "box2d/b2_collision.h"
#include "Engine/Application.h"

namespace Engine
{
	Clickable* Clickable::FocussedClickable = nullptr;
	
	Clickable::Clickable() : m_Window(Application::Get().GetRenderContext().GetWindow())
	{
		m_CursorPositionEventID = m_Window.OnCursorPosition.Bind(this, &Clickable::OnCursorPosition);
		m_MousePressedEventID = m_Window.OnMousePressed.Bind(this, &Clickable::OnMousePressed);
		m_MouseReleasedEventID = m_Window.OnMouseReleased.Bind(this, &Clickable::OnMouseReleased);
		m_ScrollEventID = m_Window.OnScroll.Bind(this, &Clickable::OnScroll);
	}

	Clickable::~Clickable()
	{
		if (FocussedClickable == this)
			FocussedClickable = nullptr;

		m_Window.OnCursorPosition.Unbind(m_CursorPositionEventID);
		m_Window.OnMousePressed.Unbind(m_MousePressedEventID);
		m_Window.OnMouseReleased.Unbind(m_MouseReleasedEventID);
		m_Window.OnScroll.Unbind(m_ScrollEventID);
	}

	void Clickable::OnEnter(const DVector2& Position)
	{
		m_IsHovered = true;

		if (!m_AlwaysFocussable)
			FocussedClickable = this;

		OnEnterEvent(Position);
	}

	void Clickable::OnHover(const DVector2& Position)
	{
		OnHoverEvent(Position);
	}

	void Clickable::OnExit(const DVector2& Position)
	{
		m_IsHovered = false;

		if (!m_IsPressed)
			FocussedClickable = nullptr;
		
		OnExitEvent(Position);
	}

	void Clickable::OnDrag(const DVector2& Position, const DVector2& Delta)
	{
		OnDraggedEvent(Position, Delta);
	}

	void Clickable::OnScroll(const DVector2& Delta)
	{
		OnScrolledEvent(Delta);
	}

	bool Clickable::OnPressed()
	{
		m_IsPressed = true;
		OnPressedEvent();

		return true;
	}

	void Clickable::OnReleased()
	{
		const DVector2 Mouse = m_Window.GetMousePosition();
		bool Close = glm::distance(
			glm::vec2(m_PressPosition.x, m_PressPosition.y),
			glm::vec2(Mouse.x, Mouse.y)
		) <= 10.f * m_Window.GetScale();

		if (m_IsHovered && Close)
		{
			OnClicked();
		}
		
		m_IsPressed = false;
		FocussedClickable = nullptr;

		OnReleasedEvent();
	}

	void Clickable::OnClicked()
	{
		OnClickedEvent();
	}

	void Clickable::OnCursorPosition(const DVector2& Position, const DVector2& Delta)
	{
		const AABB Bounds = GetBounds();
		const bool Hover = CanFocus() && Bounds.contains(Position.x, Position.y);
		
		if (Hover && !m_IsHovered)
			OnEnter(Position);
		else if (Hover)
			OnHover(Position);
		else if (!Hover && m_IsHovered)
			OnExit(Position);

		if (m_IsPressed && Delta != DVector2(0))
			OnDrag(Position, Delta);
	}

	void Clickable::OnMousePressed(uint32_t MouseButton)
	{
		if (MouseButton != 0)
			return;

		const DVector2 Mouse = m_Window.GetMousePosition();
		const DVector2 Delta = m_Window.GetMouseDelta();
		
		OnCursorPosition(Mouse, Delta);
		
		if (m_IsHovered && !m_IsPressed)
		{
			m_PressPosition = Mouse;
			OnPressed();
		}
	}

	void Clickable::OnMouseReleased(uint32_t MouseButton)
	{
		if (MouseButton != 0)
			return;
		
		if (m_IsPressed)
		{
			OnReleased();
		}
	}
}
