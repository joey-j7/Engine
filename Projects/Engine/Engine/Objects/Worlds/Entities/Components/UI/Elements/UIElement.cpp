#include "pch.h"
#include "UIElement.h"

#include "Engine/Application.h"

#include "Engine/Objects/Worlds/Entities/Components/Transform/TransformComponent2D.h"
#include "Engine/Objects/Worlds/Entities/Components/Transform/TransformComponent3D.h"

#include "Rendering/Renderers/Renderer2D.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Layouts/UILayout.h"

#include <include/effects/SkGradientShader.h>

namespace Engine
{
	UIElement::UIElement(Entity& Entity, const std::string& sName) :
		RenderComponent(Entity, sName),
		m_Window(Application::Get().GetRenderContext().GetWindow())
	{
		if (!GetEntity().GetComponent<TransformComponent3D>())
		{
			AddDependencyTypes<TransformComponent2D>();
		}

		AddProhibitedTypes<UILayout>();
		
		m_Bounds.upperBound.x = static_cast<float>(m_Width);
		m_Bounds.upperBound.y = static_cast<float>(m_Height);
	}

	UIElement::~UIElement()
	{

	}
	
	void UIElement::SetColor(const SkColor& Color)
	{
		if (m_Color == Color)
			return;

		m_Color = Color;
		m_Gradient = Gradient();
		
		MarkDirty();
	}

	void UIElement::SetGradient(const Gradient& Gradient)
	{
		if (m_Gradient == Gradient)
			return;

		m_Gradient = Gradient;

		if (Gradient.HasColors())
			m_Color = m_Gradient.GetColors()[0];

		MarkDirty();
	}

	void UIElement::SetBorder(uint32_t Width, const SkColor& Color, bool ShowFill)
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

	void UIElement::SetBorder(uint32_t Width, const Gradient& Gradient, bool ShowFill)
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

	void UIElement::ApplyAlignment(Vector2& ScreenPosition, const Vector2& ScreenScale) const
	{
		switch (m_Alignment)
		{
		case(E_ALIGN_TOP):
		case(E_ALIGN_CENTER):
		case(E_ALIGN_BOTTOM):
			ScreenPosition.x -= m_Width * ScreenScale.x * 0.5f;
			break;
		case(E_ALIGN_TOP_RIGHT):
		case(E_ALIGN_CENTER_RIGHT):
		case(E_ALIGN_BOTTOM_RIGHT):
			ScreenPosition.x -= m_Width * ScreenScale.x;
			break;
		default:
			break;
		}

		switch (m_Alignment)
		{
		case(E_ALIGN_CENTER_LEFT):
		case(E_ALIGN_CENTER):
		case(E_ALIGN_CENTER_RIGHT):
			ScreenPosition.y -= m_Height * ScreenScale.x * 0.5f;
			break;
		case(E_ALIGN_BOTTOM_LEFT):
		case(E_ALIGN_BOTTOM):
		case(E_ALIGN_BOTTOM_RIGHT):
			ScreenPosition.y -= m_Height * ScreenScale.x;
			break;
		default:
			break;
		}
	}

	UILayout* UIElement::CheckParentLayout(Entity& Origin) const
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

	void UIElement::ApplyAnchor(Vector2& ScreenPosition)
	{
		Entity* Parent = GetEntity().GetParent();
		Vector2 Dims(0.f);
		
		if (Parent)
		{
			if (UILayout* Layout = CheckParentLayout(*Parent))
			{
				AABB Bounds = Layout->GetBounds();

				ScreenPosition += Bounds.lowerBound;

				Dims.x = Bounds.upperBound.x - Bounds.lowerBound.x;
				Dims.y = Bounds.upperBound.y - Bounds.lowerBound.y;
			}
		}
		
		if (Dims == Vector2(0.f))
		{
			Window& Window = Application::Get().GetRenderContext().GetWindow();
			Dims = Vector2(Window.GetWidth(), Window.GetHeight());
		}

		switch (m_Anchor)
		{
		case(E_ANCH_TOP):
		case(E_ANCH_CENTER):
		case(E_ANCH_BOTTOM):
		case(E_ANCH_CENTER_FILL_VERTICAL):
			ScreenPosition.x += Dims.x * 0.5f;
			break;
		case(E_ANCH_TOP_RIGHT):
		case(E_ANCH_CENTER_RIGHT):
		case(E_ANCH_BOTTOM_RIGHT):
		case(E_ANCH_RIGHT_FILL):
			ScreenPosition.x += Dims.x - ScreenPosition.x - m_RightOffset;
			break;
		default:
			break;
		}

		switch (m_Anchor)
		{
		case(E_ANCH_CENTER_LEFT):
		case(E_ANCH_CENTER):
		case(E_ANCH_CENTER_RIGHT):
		case(E_ANCH_CENTER_FILL_HORIZONTAL):
			ScreenPosition.y += Dims.y * 0.5f;
			break;
		case(E_ANCH_BOTTOM_LEFT):
		case(E_ANCH_BOTTOM):
		case(E_ANCH_BOTTOM_RIGHT):
		case(E_ANCH_BOTTOM_FILL):
			ScreenPosition.y += Dims.y - ScreenPosition.y - m_BottomOffset;
			break;
		default:
			break;
		}

		switch (m_Anchor)
		{
		case(E_ANCH_TOP_FILL):
		case(E_ANCH_CENTER_FILL_HORIZONTAL):
		case(E_ANCH_BOTTOM_FILL):
			SetWidth(static_cast<uint32_t>(Dims.x - ScreenPosition.x - m_RightOffset));
			break;
		case(E_ANCH_LEFT_FILL):
		case(E_ANCH_CENTER_FILL_VERTICAL):
		case(E_ANCH_RIGHT_FILL):
			SetHeight(static_cast<uint32_t>(Dims.y - ScreenPosition.y - m_BottomOffset));
			break;
		case(E_ANCH_FULL_FILL):
			SetSize(
				static_cast<uint32_t>(Dims.x - ScreenPosition.x - m_RightOffset),
				static_cast<uint32_t>(Dims.y - ScreenPosition.y - m_BottomOffset)
			);
			break;
		default:
			break;
		}
	}

	void UIElement::BeginDraw()
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
			m_Paint.setStrokeWidth(
				static_cast<SkScalar>(m_BorderWidth)
			);
		}
		
		// Transform
		Vector2 ScreenPosition = GetPosition();
		const float ScreenRotation = GetRotation();
		Vector2 ScreenScale = GetScale();

		if (m_ScaleWithDPI)
		{
			ScreenPosition *= m_Window.GetScale();
			ScreenScale *= m_Window.GetScale();
		}

		// Anchor and alignment
		ApplyAnchor(ScreenPosition);
		ApplyAlignment(ScreenPosition, ScreenScale);

		// Apply
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

	void UIElement::EndDraw()
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
	
	void UIElement::SetWidth(uint32_t Width)
	{
		if (m_Width == Width)
			return;

		m_Width = Width;
		
		m_Bounds.lowerBound.x = 0.f;
		m_Bounds.upperBound.x = static_cast<float>(Width);
		
		MarkDirty();
	}

	void UIElement::SetHeight(uint32_t Height)
	{
		if (m_Height == Height)
			return;

		m_Height = Height;
		
		m_Bounds.lowerBound.y = 0.f;
		m_Bounds.upperBound.y = static_cast<float>(Height);
		
		MarkDirty();
	}

	void UIElement::SetSize(uint32_t Width, uint32_t Height)
	{
		SetWidth(Width);
		SetHeight(Height);
	}

	void UIElement::ScaleWithDPI(bool Scale)
	{
		if (m_ScaleWithDPI == Scale)
			return;

		m_ScaleWithDPI = Scale;
		MarkDirty();
	}

	void UIElement::SetAntialiasing(bool AA)
	{
		m_UseAntialiasing = AA;
	}

	void UIElement::SetAlignment(Alignment Alignment)
	{
		if (m_Alignment == Alignment)
			return;

		m_Alignment = Alignment;
		
		MarkDirty();
	}

	void UIElement::SetAnchor(Anchor Anchor)
	{
		if (m_Anchor == Anchor)
			return;

		m_Anchor = Anchor;

		MarkDirty();
	}

	const AABB UIElement::GetBounds()
	{
		AABB Bounds = m_Bounds;

		Vector2 ScreenScale = GetScale();
		Vector2 ScreenPosition = GetPosition();

		if (m_ScaleWithDPI)
		{
			ScreenPosition *= m_Window.GetScale();
			ScreenScale *= m_Window.GetScale();
		}

		// Anchor and alignment
		ApplyAnchor(ScreenPosition);
		ApplyAlignment(ScreenPosition, ScreenScale);
		
		Bounds.lowerBound.x *= ScreenScale.x;
		Bounds.lowerBound.y *= ScreenScale.y;
		Bounds.upperBound.x *= ScreenScale.x;
		Bounds.upperBound.y *= ScreenScale.y;
		
		Bounds.lowerBound += ScreenPosition;
		Bounds.upperBound += ScreenPosition;

		Bounds.lowerBound += m_LayoutOffset;
		Bounds.upperBound += m_LayoutOffset;
		
		return Bounds;
	}
	
	void UIElement::SetLayoutOffset(const Vector2& Offset)
	{
		if (m_LayoutOffset == Offset)
			return;

		m_LayoutOffset = Offset;
		MarkDirty();
	}
}
