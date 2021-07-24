#include "pch.h"
#include "Clickable.h"

#include "box2d/b2_collision.h"
#include "Engine/Application.h"

namespace Engine
{
	Clickable::Clickable() : m_Window(Application::Get().GetRenderContext().GetWindow())
	{
		m_Window.OnCursorPosition.Bind(this, &Clickable::OnCursorPosition);
		m_Window.OnMousePressed.Bind(this, &Clickable::OnMousePressed);
		m_Window.OnMouseReleased.Bind(this, &Clickable::OnMouseReleased);
	}

	Clickable::~Clickable()
	{
	}

	void Clickable::OnEnter(double XPosition, double YPosition)
	{
		m_IsHovered = true;
		CB_CORE_INFO("Enter");
	}

	void Clickable::OnHover(double XPosition, double YPosition)
	{
		
	}

	void Clickable::OnExit(double XPosition, double YPosition)
	{
		m_IsHovered = false;
		CB_CORE_INFO("Exit");
	}

	void Clickable::OnPressed()
	{
		m_IsPressed = true;
	}

	void Clickable::OnReleased()
	{
		if (m_IsHovered)
		{
			OnClicked();
		}

		m_IsPressed = false;
	}

	void Clickable::OnClicked()
	{

	}

	void Clickable::OnCursorPosition(double XPosition, double YPosition)
	{
		const AABB Bounds = GetBounds();
		
		const bool Hover = CollisionCheck::Contains(Bounds, Vector2(XPosition, YPosition));
		
		if (Hover && !m_IsHovered)
			OnEnter(XPosition, YPosition);
		else if (Hover)
			OnHover(XPosition, YPosition);
		else if (!Hover && m_IsHovered)
			OnExit(XPosition, YPosition);
	}

	void Clickable::OnMousePressed(uint32_t MouseButton)
	{
		if (MouseButton != 0)
			return;
		
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
