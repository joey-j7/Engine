#include "pch.h"
#include "Clickable.h"

#include "box2d/b2_collision.h"
#include "Engine/Application.h"

namespace Engine
{
	Clickable* Clickable::PressedClickable = nullptr;
	
	Clickable::Clickable() : m_Window(Application::Get().GetRenderContext().GetWindow())
	{
		m_Window.OnCursorPosition.Bind(this, &Clickable::OnCursorPosition);
		m_Window.OnMousePressed.Bind(this, &Clickable::OnMousePressed);
		m_Window.OnMouseReleased.Bind(this, &Clickable::OnMouseReleased);
	}

	Clickable::~Clickable()
	{
	}

	void Clickable::OnEnter(const DVector2& Position)
	{
		m_IsHovered = true;
		OnEnterEvent(Position);
	}

	void Clickable::OnHover(const DVector2& Position)
	{
		OnHoverEvent(Position);
	}

	void Clickable::OnExit(const DVector2& Position)
	{
		m_IsHovered = false;
		OnExitEvent(Position);
	}

	bool Clickable::OnPressed()
	{
		if (!CanPress())
			return false;

		PressedClickable = this;
		
		m_IsPressed = true;
		OnPressedEvent();

		return true;
	}

	void Clickable::OnReleased()
	{
		PressedClickable = nullptr;

		if (m_IsHovered)
		{
			OnClicked();
		}
		
		m_IsPressed = false;
		OnReleasedEvent();
	}

	void Clickable::OnClicked()
	{
		OnClickedEvent();
	}

	void Clickable::OnCursorPosition(const DVector2& Position)
	{
		const AABB Bounds = GetBounds();
		
		const bool Hover = Bounds.contains(Position.x, Position.y);
		
		if (Hover && !m_IsHovered)
			OnEnter(Position);
		else if (Hover)
			OnHover(Position);
		else if (!Hover && m_IsHovered)
			OnExit(Position);
	}

	void Clickable::OnMousePressed(uint32_t MouseButton)
	{
		if (MouseButton != 0)
			return;

		const DVector2 Mouse = m_Window.GetMousePosition();
		OnCursorPosition(Mouse);
		
		if (m_IsHovered && !m_IsPressed)
		{
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
