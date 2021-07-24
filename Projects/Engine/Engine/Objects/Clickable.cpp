#include "pch.h"
#include "Clickable.h"

#include "box2d/b2_collision.h"
#include "Engine/Application.h"

namespace Engine
{
	Clickable::Clickable() : m_Window(Application::Get().GetRenderContext().GetWindow())
	{
		m_Window.OnCursorPosition.Bind(this, &Clickable::OnCursorPosition);
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

	void Clickable::OnPressed(double XPosition, double YPosition)
	{
		m_IsPressed = true;
	}

	void Clickable::OnReleased(double XPosition, double YPosition)
	{
		if (m_IsHovered)
		{
			OnClicked(XPosition, YPosition);
		}

		m_IsPressed = false;
	}

	void Clickable::OnClicked(double XPosition, double YPosition)
	{

	}

	void Clickable::OnCursorPosition(double XPosition, double YPosition)
	{
		const AABB Bounds = GetBounds();
		
		const bool Hover = CollisionCheck::Contains(Bounds, Vector2(XPosition, YPosition));
		const bool Press = m_Window.IsMousePressed();
		
		if (Hover && !m_IsHovered)
			OnEnter(XPosition, YPosition);
		else if (Hover)
			OnHover(XPosition, YPosition);
		else if (!Hover && m_IsHovered)
			OnExit(XPosition, YPosition);

		if (Press && !m_IsPressed)
			OnPressed(XPosition, YPosition);
		else if (!Press && m_IsPressed)
			OnReleased(XPosition, YPosition);
	}
}
