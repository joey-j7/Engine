#include "pch.h"
#include "UIElement.h"

#include "Engine/Application.h"

#include "Engine/Objects/Worlds/Entities/Components/Transform/Transform2DComponent.h"
#include "Engine/Objects/Worlds/Entities/Components/Transform/Transform3DComponent.h"

#include "Rendering/Renderers/Renderer2D.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Layouts/UILayout.h"

#include <include/effects/SkGradientShader.h>
#include <src/core/SkRuntimeEffectPriv.h>

namespace Engine
{
	UIElement::UIElement(Entity& Entity, const String& sName) :
		Render2DComponent(Entity, sName),
		m_Window(Application::Get().GetRenderContext().GetWindow())
	{
		AddProhibitedTypes<UILayout>();

		m_Bounds.fLeft = 0.f;
		m_Bounds.fTop = 0.f;
		m_Bounds.fRight = static_cast<float>(m_Width);
		m_Bounds.fBottom = static_cast<float>(m_Height);
	}

	UIElement::~UIElement()
	{

	}
	
	void UIElement::SetColor(const Color& Color)
	{
		if (m_Color == Color)
			return;

		m_Color = Color;
		m_Gradient = Gradient();

		if (m_PaintStyle == SkPaint::Style::kStroke_Style)
			m_BorderColor = Color;
		
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

	void UIElement::SetBorder(uint32_t Width, const Color& Color, bool ShowFill)
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

	Vector2 UIElement::CalcPivot(const Vector2& ScreenScale) const
	{
		Vector2 Offset;
		
		Offset.x = -glm::max(static_cast<float>(m_Width), m_Padding.z + m_Padding.x) * m_Pivot.x * ScreenScale.x;
		Offset.y = -glm::max(static_cast<float>(m_Height), m_Padding.w + m_Padding.y) * m_Pivot.y * ScreenScale.y;

		return Offset;
	}

	Vector2 UIElement::CalcAnchor(const Vector2& ScreenScale)
	{
		Vector2 Offset;

		Entity* Parent = GetEntity().GetParent();
		Vector2 Dims(0.f);
		
		if (Parent)
		{
			AABB Bounds;
			
			if (UILayout* Layout = CheckParentLayout(*Parent))
			{
				Bounds = Layout->GetBounds();

				Offset += Vector2(Bounds.fLeft, Bounds.fTop);

				Dims.x = Bounds.fRight - Bounds.fLeft;
				Dims.y = Bounds.fBottom - Bounds.fTop;
			}
			else
			{
				Bounds = Parent->GetBounds();

				Offset += Vector2(Bounds.fLeft, Bounds.fTop);

				Dims.x = Bounds.fRight - Bounds.fLeft;
				Dims.y = Bounds.fBottom - Bounds.fTop;
			}
		}
		
		if (Dims == Vector2(0.f))
		{
			Window& Window = Application::Get().GetRenderContext().GetWindow();
			const float Scale = 1.f;// / Window.GetScale();
			Dims = Vector2(Window.GetWidth() * Scale, Window.GetHeight() * Scale);
		}
		
		switch (m_Anchor)
		{
		case(E_ANCH_TOP):
		case(E_ANCH_CENTER):
		case(E_ANCH_BOTTOM):
		case(E_ANCH_CENTER_FILL_VERTICAL):
			Offset.x += Dims.x * 0.5f;
			break;
		case(E_ANCH_TOP_RIGHT):
		case(E_ANCH_CENTER_RIGHT):
		case(E_ANCH_BOTTOM_RIGHT):
		case(E_ANCH_RIGHT_FILL):
			Offset.x += Dims.x - m_RightOffset * ScreenScale.x;
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
			Offset.y += Dims.y * 0.5f;
			break;
		case(E_ANCH_BOTTOM_LEFT):
		case(E_ANCH_BOTTOM):
		case(E_ANCH_BOTTOM_RIGHT):
		case(E_ANCH_BOTTOM_FILL):
			Offset.y += Dims.y - m_BottomOffset * ScreenScale.y;
			break;
		default:
			break;
		}

		switch (m_Anchor)
		{
		case(E_ANCH_TOP_FILL):
		case(E_ANCH_CENTER_FILL_HORIZONTAL):
		case(E_ANCH_BOTTOM_FILL):
			SetWidth(static_cast<uint32_t>(Dims.x - m_RightOffset * ScreenScale.x));
			break;
		case(E_ANCH_LEFT_FILL):
		case(E_ANCH_CENTER_FILL_VERTICAL):
		case(E_ANCH_RIGHT_FILL):
			SetHeight(static_cast<uint32_t>(Dims.y - m_BottomOffset * ScreenScale.y));
			break;
		case(E_ANCH_FULL_FILL):
			SetSize(
				static_cast<uint32_t>(Dims.x - m_RightOffset * ScreenScale.x),
				static_cast<uint32_t>(Dims.y - m_BottomOffset * ScreenScale.y)
			);
			break;
		default:
			break;
		}

		return Offset;
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

			AABB Bounds = GetUnscaledBounds();

			switch (m_Gradient.GetType())
			{
			case Gradient::E_HORIZONTAL:
				Points[1] = {
				Bounds.width(), 0.f
				};
				break;
			case Gradient::E_VERTICAL:
				Points[1] = {
				0.f, Bounds.height()
				};
				break;
			case Gradient::E_DIAGONAL:
				Points[1] = {
				Bounds.width(),
				Bounds.height()
				};
				break;
			case Gradient::E_NONE:
			default:
				break;
			}

			SkColor Color = m_Gradient.GetColors()[0];
			
			m_Paint.setShader(
				SkGradientShader::MakeLinear(
					Points,
					&Color,
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
			m_Gradient.HasColors() ?
				m_Gradient == m_BorderGradient :
				m_Color == m_BorderColor
		;
		
		m_Paint.setStroke(HasIdenticalStroke);

		if (HasIdenticalStroke)
		{
			m_Paint.setStrokeWidth(
				static_cast<SkScalar>(m_BorderWidth)
			);
		}
		
		// Compiles shader if necessary
		SkShader* Shader = GetShader();
		
		// Set all shaders (fragment, vertex...)
		if (Shader)
		{
			m_Paint.setShader(m_RuntimeShaderInfo.m_Shader);
		}
		
		// Transform
		m_Canvas->setMatrix(GetMatrix());
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

				SkColor Color = m_BorderGradient.GetColors()[0];
				
				m_Paint.setShader(
					SkGradientShader::MakeLinear(
						Points,
						&Color,
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
		
		m_Bounds.fLeft = 0.f;
		m_Bounds.fRight = static_cast<float>(Width);
		
		MarkDirty();
	}

	void UIElement::SetHeight(uint32_t Height)
	{
		if (m_Height == Height)
			return;

		m_Height = Height;
		
		m_Bounds.fTop = 0.f;
		m_Bounds.fBottom = static_cast<float>(Height);
		
		MarkDirty();
	}

	const SkMatrix& UIElement::GetMatrix()
	{
		const Vector2 ScreenPosition = GetPosition(true);
		const float ScreenRotation = GetRotation();
		Vector2 ScreenScale = GetScale();

		if (m_ScaleWithDPI)
		{
			ScreenScale *= m_Window.GetScale();
		}
		
		// Apply screen anchor
		const Vector2 Anchor = CalcAnchor(Vector2(1.f));
		m_Matrix = SkMatrix::Translate(Anchor.x, Anchor.y);

		m_Matrix = SkMatrix::Concat(m_Matrix, SkMatrix::Scale(ScreenScale.x, ScreenScale.y));
		m_Matrix = SkMatrix::Concat(m_Matrix, SkMatrix::RotateDeg(ScreenRotation));

		// Apply object pivot
		const Vector2 Pivot = CalcPivot(Vector2(1.f));
		m_Matrix = SkMatrix::Concat(m_Matrix, SkMatrix::Translate(Pivot.x, Pivot.y));

		m_Matrix = SkMatrix::Concat(m_Matrix, SkMatrix::Translate(ScreenPosition.x, ScreenPosition.y));

		return m_Matrix;
	}

	void UIElement::SetSize(uint32_t Width, uint32_t Height)
	{
		SetWidth(Width);
		SetHeight(Height);
	}

	void UIElement::SetBorderWidth(uint32_t Thickness)
	{
		if (m_BorderWidth == Thickness)
			return;

		m_BorderWidth = Thickness;
		MarkDirty();
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

	void UIElement::SetPivot(const Vector2& Pivot)
	{
		Vector2 CheckedPivot;
		
		CheckedPivot.x = glm::clamp(Pivot.x, 0.f, 1.f);
		CheckedPivot.y = glm::clamp(Pivot.y, 0.f, 1.f);

		if (m_Pivot == CheckedPivot)
			return;

		m_Pivot = CheckedPivot;

		MarkDirty();
	}

	void UIElement::SetAnchor(Anchor Anchor)
	{
		if (m_Anchor == Anchor)
			return;

		m_Anchor = Anchor;

		MarkDirty();
	}
	
	SkShader* UIElement::GetShader()
	{
		if (m_RuntimeShaderInfo.m_Builder)
		{
			if (m_RuntimeShaderInfo.m_Dirty)
			{
				auto Shader = m_RuntimeShaderInfo.m_Builder->makeShader(
					nullptr,
					false
				);

				if (!Shader)
				{
					CB_CORE_ERROR("fShaderBuilder->makeShader failed");
				}
				else
				{
					if (m_RuntimeShaderInfo.m_Shader.get())
						m_RuntimeShaderInfo.m_Shader.reset();

					m_RuntimeShaderInfo.m_Shader = Shader;
					m_RuntimeShaderInfo.m_Dirty = false;
				}
			}

			return m_RuntimeShaderInfo.m_Shader.get();
		}

		return nullptr;
	}

	void UIElement::SetShader(const String& Source)
	{
		SkRuntimeEffect::Options Options;
		SkRuntimeEffectPriv::EnableFragCoord(&Options);
		
		auto [Effect, Error] = SkRuntimeEffect::MakeForShader(SkString(Source.c_str()), Options);

		if (!Effect)
		{
			CB_CORE_ERROR("Error compiling shader: {0}", Error.c_str());
			return;
		}
		
		if (m_RuntimeShaderInfo.m_Builder)
			delete m_RuntimeShaderInfo.m_Builder;

		m_RuntimeShaderInfo.m_Builder = new SkRuntimeShaderBuilder(Effect);
		m_RuntimeShaderInfo.m_Dirty = true;
	}
	
	void UIElement::SetPadding(const Vector4& Pad)
	{
		if (m_Padding == Pad)
			return;

		m_Padding = Pad;

		MarkDirty();
	}

	const AABB UIElement::GetUnscaledBounds()
	{
		SkMatrix Matrix = GetMatrix();
		Matrix = SkMatrix::Concat(SkMatrix::Scale(1.f / Matrix.getScaleX(), 1.f / Matrix.getScaleY()), Matrix);

		AABB Bounds;
		Matrix.mapRect(&Bounds, m_Bounds);

		const float Height = glm::max(Bounds.fBottom - Bounds.fTop, m_Padding.x + m_Padding.z);
		const float Width = glm::max(Bounds.fRight - Bounds.fLeft, m_Padding.x + m_Padding.z);

		Bounds.fBottom = Bounds.fTop + Height;
		Bounds.fRight = Bounds.fLeft + Width;

		return Bounds;
	}

	const AABB UIElement::GetBounds()
	{
		SkMatrix Matrix = GetMatrix();
		
		AABB Bounds;
		Matrix.mapRect(&Bounds, m_Bounds);

		float Scale = 1.f;

		if (m_ScaleWithDPI)
			Scale = m_Window.GetScale();

		const float Height = glm::max(Bounds.fBottom - Bounds.fTop, (m_Padding.x + m_Padding.z) * Scale);
		const float Width = glm::max(Bounds.fRight - Bounds.fLeft, (m_Padding.x + m_Padding.z) * Scale);

		Bounds.fBottom = Bounds.fTop + Height;
		Bounds.fRight = Bounds.fLeft + Width;
		
		return Bounds;
	}
	
	void UIElement::SetElementShader(sk_sp<SkShader> Shader)
	{
		if (m_ElementShader.get() == Shader.get())
			return;
		
		if (m_ElementShader.get())
			m_ElementShader.reset();

		m_ElementShader = Shader;

		if (m_RuntimeShaderInfo.m_Builder)
		{
			m_RuntimeShaderInfo.m_Builder->child("Element") = m_ElementShader;
		}
		
		m_RuntimeShaderInfo.m_Dirty = true;
		
		MarkDirty();
	}
}
