#include "pch.h"
#include "UIComponent.h"

#include "Engine/Application.h"

#include "Engine/Objects/Worlds/Entities/Components/Transform/TransformComponent2D.h"
#include "Engine/Objects/Worlds/Entities/Components/Transform/TransformComponent3D.h"

#include "Rendering/Renderers/Renderer2D.h"

#include <include/effects/SkGradientShader.h>

namespace Engine
{
	UIComponent::UIComponent(Entity& Entity, const std::string& sName) : RenderComponent(Entity, sName)
	{
		if (!GetEntity().GetComponent<TransformComponent3D>())
		{
			AddDependencyTypes<TransformComponent2D>();
		}

		m_Bounds.upperBound.x = m_Width;
		m_Bounds.upperBound.y = m_Height;
	}

	UIComponent::~UIComponent()
	{

	}
	
	const Vector2& UIComponent::GetPosition() const
	{
		switch (m_RenderDimension)
		{
		case E_3D:
			return m_Entity.GetComponent<TransformComponent3D>()->GetPosition();
		case E_2D:
		case E_NONE:
		default:
			return m_Entity.GetComponent<TransformComponent2D>()->GetPosition();
		}
	}

	float UIComponent::GetRotation() const
	{
		switch (m_RenderDimension)
		{
		case E_3D:
			return m_Entity.GetComponent<TransformComponent3D>()->GetRotation().x;
		case E_2D:
		case E_NONE:
		default:
			return m_Entity.GetComponent<TransformComponent2D>()->GetRotation();
		}
	}

	const Vector2& UIComponent::GetScale() const
	{
		switch (m_RenderDimension)
		{
		case E_3D:
			return m_Entity.GetComponent<TransformComponent3D>()->GetScale();
		case E_2D:
		case E_NONE:
		default:
			return m_Entity.GetComponent<TransformComponent2D>()->GetScale();
		}
	}

	void UIComponent::SetColor(const SkColor& Color)
	{
		if (m_Color == Color)
			return;

		m_Color = Color;
		m_Gradient = Gradient();
		
		MarkDirty();
	}

	void UIComponent::SetGradient(const Gradient& Gradient)
	{
		if (m_Gradient == Gradient)
			return;

		m_Gradient = Gradient;

		if (Gradient.HasColors())
			m_Color = m_Gradient.GetColors()[0];

		MarkDirty();
	}

	void UIComponent::SetBorder(uint32_t Width, const SkColor& Color, bool ShowFill)
	{
		if (m_BorderWidth == Width && m_BorderColor == Color && m_ShowFill == ShowFill)
			return;

		m_ShowFill = ShowFill && Width > 0;
		m_BorderColor = Color;
		m_BorderGradient = Gradient();
		m_BorderWidth = Width;
		
		m_PaintStyle = Width > 0 ?
			m_ShowFill ?
			SkPaint::Style::kStrokeAndFill_Style :
			SkPaint::Style::kStroke_Style :
			SkPaint::Style::kFill_Style
		;
		
		if (m_PaintStyle == SkPaint::Style::kStroke_Style)
		{
			m_Color = m_BorderColor;
			m_Gradient = m_BorderGradient;
		}
		
		MarkDirty();
	}

	void UIComponent::SetBorder(uint32_t Width, const Gradient& Gradient, bool ShowFill)
	{
		if (m_BorderWidth == Width && m_BorderGradient == Gradient && m_ShowFill == ShowFill)
			return;

		m_ShowFill = ShowFill && Width > 0;
		m_BorderGradient = Gradient;

		if (m_BorderGradient.HasColors())
			m_BorderColor = *Gradient.GetColors().begin();
		
		m_BorderWidth = Width;

		m_PaintStyle = Width > 0 ?
			m_ShowFill ?
			SkPaint::Style::kStrokeAndFill_Style :
			SkPaint::Style::kStroke_Style :
			SkPaint::Style::kFill_Style
		;
		
		if (m_PaintStyle == SkPaint::Style::kStroke_Style)
		{
			m_Color = m_BorderColor;
			m_Gradient = m_BorderGradient;
		}

		MarkDirty();
	}

	void UIComponent::BeginDraw()
	{
		m_Canvas = Application::Get().GetRenderContext().GetAPI().GetRenderer2D()->GetCanvas();
		m_Canvas->save();

		m_Paint = SkPaint();
		m_Paint.setAntiAlias(m_UseAntialiasing);

		// Gradient
		if (m_Gradient.GetType() != Gradient::Type::E_NONE)
		{
			SkPoint Points[2] = {
				{ 0, 0 },
				{ 0, 0 }
			};

			switch (m_Gradient.GetType())
			{
			case Gradient::E_HORIZONTAL:
				Points[1] = {
				static_cast<SkScalar>(m_Width), 0.f
				};
				break;
			case Gradient::E_VERTICAL:
				Points[1] = {
				0.f, static_cast<SkScalar>(m_Height)
				};
				break;
			case Gradient::E_DIAGONAL:
				Points[1] = {
				static_cast<SkScalar>(m_Width), static_cast<SkScalar>(m_Height)
				};
				break;
			case Gradient::E_NONE:
			default:
				break;
			}

			m_Paint.setShader(
				SkGradientShader::MakeLinear(
					Points,
					&m_Gradient.GetColors()[0],
					&m_Gradient.GetPositions()[0],
					m_Gradient.GetColors().size(),
					SkTileMode::kClamp
				)
			);
		}
		// Single color
		else
		{
			m_Paint.setColor(m_Color);
		}

		// Stroke logic
		const bool HasIdenticalStroke = m_PaintStyle != SkPaint::Style::kFill_Style &&
		(
			m_Gradient.HasColors() ?
			m_Gradient == m_BorderGradient :
			m_Color == m_BorderColor
		);
		
		m_Paint.setStroke(HasIdenticalStroke);

		if (HasIdenticalStroke)
		{
			m_Paint.setStrokeWidth(m_BorderWidth);
		}
		
		// Transform
		const Vector2& ScreenPosition = GetPosition();
		const float ScreenRotation = GetRotation();
		const Vector2& ScreenScale = GetScale();
		
		m_Canvas->translate(
			ScreenPosition.x,
			ScreenPosition.y
		);

		m_Canvas->rotate(
			ScreenRotation
		);

		m_Canvas->scale(
			ScreenScale.x,
			ScreenScale.y
		);
	}

	void UIComponent::EndDraw()
	{
		if (m_PaintStyle == SkPaint::Style::kStrokeAndFill_Style &&
			(
				m_Gradient.HasColors() ?
				m_Gradient != m_BorderGradient :
				m_Color != m_BorderColor
			)
		)
		{
			m_Paint.setStroke(true);

			m_Paint.setStyle(SkPaint::Style::kStroke_Style);
			m_Paint.setStrokeWidth(m_BorderWidth);
			
			// Gradient
			if (m_BorderGradient.GetType() != Gradient::Type::E_NONE)
			{
				SkPoint Points[2] = {
					{ 0, 0 },
					{ 0, 0 }
				};

				switch (m_BorderGradient.GetType())
				{
				case Gradient::E_HORIZONTAL:
					Points[1] = {
					static_cast<SkScalar>(m_Width), 0.f
					};
					break;
				case Gradient::E_VERTICAL:
					Points[1] = {
					0.f, static_cast<SkScalar>(m_Height)
					};
					break;
				case Gradient::E_DIAGONAL:
					Points[1] = {
					static_cast<SkScalar>(m_Width), static_cast<SkScalar>(m_Height)
					};
					break;
				case Gradient::E_NONE:
				default:
					break;
				}

				m_Paint.setShader(
					SkGradientShader::MakeLinear(
						Points,
						&m_BorderGradient.GetColors()[0],
						&m_BorderGradient.GetPositions()[0],
						m_BorderGradient.GetColors().size(),
						SkTileMode::kClamp
					)
				);
			}
			// Single color
			else
			{
				m_Paint.setShader(nullptr);
				m_Paint.setColor(m_BorderColor);
			}

			Draw();
		}

		m_Canvas->restore();
	}
	
	void UIComponent::SetWidth(uint32_t Width)
	{
		if (m_Width == Width)
			return;

		m_Width = Width;
		m_Bounds.lowerBound.x = 0.f;
		m_Bounds.upperBound.x = static_cast<float>(Width);
		
		MarkDirty();
	}

	void UIComponent::SetHeight(uint32_t Height)
	{
		if (m_Height == Height)
			return;

		m_Height = Height;
		m_Bounds.lowerBound.y = 0.f;
		m_Bounds.upperBound.y = static_cast<float>(Height);
		
		MarkDirty();
	}

	void UIComponent::SetSize(uint32_t Width, uint32_t Height)
	{
		SetWidth(Width);
		SetHeight(Height);
	}

	void UIComponent::SetAntialiasing(bool AA)
	{
		m_UseAntialiasing = AA;
	}

	const AABB UIComponent::GetBounds() const
	{
		AABB Bounds = m_Bounds;
		
		Bounds.lowerBound += Vector2(GetPosition());
		Bounds.upperBound += Vector2(GetPosition());
		
		return Bounds;
	}
}
