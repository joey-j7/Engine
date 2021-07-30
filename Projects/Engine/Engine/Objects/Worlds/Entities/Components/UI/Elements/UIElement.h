#pragma once

#include <map>

#include "Engine/Objects/Worlds/Entities/Components/RenderComponent.h"

#include <include/core/SkCanvas.h>

#include <include/effects/SkRuntimeEffect.h>

namespace Engine
{
	class Window;
	class Renderer2D;
	class UILayout;

	enum Anchor
	{
		E_ANCH_TOP_LEFT = 0,
		E_ANCH_TOP = 1,
		E_ANCH_TOP_RIGHT = 2,
		E_ANCH_TOP_FILL = 3,
		E_ANCH_CENTER_LEFT = 4,
		E_ANCH_CENTER = 5,
		E_ANCH_CENTER_RIGHT = 6,
		E_ANCH_CENTER_FILL_HORIZONTAL = 7,
		E_ANCH_CENTER_FILL_VERTICAL = 8,
		E_ANCH_BOTTOM_LEFT = 9,
		E_ANCH_BOTTOM = 10,
		E_ANCH_BOTTOM_RIGHT = 11,
		E_ANCH_BOTTOM_FILL = 12,
		E_ANCH_LEFT_FILL = 13,
		E_ANCH_RIGHT_FILL = 14,
		E_ANCH_FULL_FILL = 15
	};
	
	class Engine_API UIElement : public Render2DComponent
	{
		friend class UILayout;
		
	public:
		struct Gradient
		{
			enum Type
			{
				E_NONE = 0,
				E_HORIZONTAL = 1,
				E_VERTICAL = 2,
				E_DIAGONAL = 3
			};
			
			Gradient() {}
			Gradient(
				const std::map<SkScalar, SkColor>& PositionsColors,
				Type Type = E_HORIZONTAL
			)
			{
				if (PositionsColors.size() > 1)
					m_Type = Type;
				else
					CB_CORE_WARN("Specified a gradient with insufficient amount of colors!");

				if (m_Type != E_NONE)
				{
					m_Colors.reserve(PositionsColors.size());
					m_Positions.reserve(PositionsColors.size());
					
					const SkScalar LargestInv = 1.f / std::max(1.f, std::prev(PositionsColors.end())->first);
					
					for (auto& Pair : PositionsColors)
					{
						m_Positions.push_back(Pair.first * LargestInv);
						m_Colors.push_back(Pair.second);
					}
				}
			}

			Type GetType() const { return m_Type; }
			bool HasColors() const { return m_Type != E_NONE; }
			
			const std::vector<SkColor>& GetColors() const { return m_Colors; }
			const std::vector<SkScalar>& GetPositions() const { return m_Positions; }
			
			bool operator==(const Gradient& Other) const
			{
				return m_Colors == Other.m_Colors &&
					m_Positions == Other.m_Positions &&
					m_Type == Other.m_Type
				;
			}

			bool operator!=(const Gradient& Other) const
			{
				return m_Colors != Other.m_Colors &&
					m_Positions != Other.m_Positions &&
					m_Type != Other.m_Type
				;
			}

		private:
			Type m_Type = E_NONE;

			std::vector<SkColor> m_Colors;
			std::vector<SkScalar> m_Positions;
		};
		
		UIElement(Entity& Entity, const std::string& sName = "UI Component");
		virtual ~UIElement();

		const SkColor& GetColor() const { return m_Color; }
		void SetColor(const SkColor& Color);

		const Gradient& GetGradient() const { return m_Gradient; }
		void SetGradient(const Gradient& Gradient);

		const SkPaint::Style& GetStyle() const { return m_PaintStyle; }

		void SetBorder(uint32_t Width, const SkColor& Color = SK_ColorBLACK, bool ShowFill = true);
		void SetBorder(uint32_t Width, const Gradient& Gradient, bool ShowFill = true);

		uint32_t GetWidth() const { return m_Width; }
		void SetWidth(uint32_t Width);

		uint32_t GetHeight() const { return m_Height; }
		void SetHeight(uint32_t Height);

		void SetSize(uint32_t Width, uint32_t Height);

		bool ScalesWithDPI() const { return m_ScaleWithDPI; }
		void ScaleWithDPI(bool Scale);

		bool GetAntialiasing() const { return m_UseAntialiasing; }
		virtual void SetAntialiasing(bool AA);

		const Vector2& GetAlignment() const { return m_Alignment; }
		void SetAlignment(const Vector2& Alignment);

		Anchor GetAnchor() const { return m_Anchor; }
		void SetAnchor(Anchor Anchor);

		SkShader* GetShader();
		void SetShader(const std::string& Source);

		template <typename T>
		void SetShaderUniform(const std::string& UniformName, const T Value[], const int32_t Count)
		{
			if (!m_RuntimeShaderInfo.m_Builder)
			{
				CB_CORE_WARN("Cannot set uniform because shader is not present");
				return;
			}

			m_RuntimeShaderInfo.m_Builder->uniform(UniformName.c_str()).set(&Value, Count);
			m_RuntimeShaderInfo.m_Dirty = true;

			MarkDirty();
		}

		const Vector4& GetPadding() const { return m_Padding; }
		void SetPadding(const Vector4& Pad);

		virtual const AABB GetUnscaledBounds();
		virtual const AABB GetBounds() override;

	protected:
		void SetElementShader(sk_sp<SkShader> Shader);
		
		SkCanvas* m_Canvas = nullptr;
		SkPaint m_Paint;

		struct RuntimeShaderInfo
		{
			~RuntimeShaderInfo() { delete m_Builder; }
			
			sk_sp<SkShader> m_Shader;
			SkRuntimeShaderBuilder* m_Builder = nullptr;
			bool m_Dirty = false;
		};

		sk_sp<SkShader> m_ElementShader;
		RuntimeShaderInfo m_RuntimeShaderInfo;
		
		SkMatrix m_Matrix;

		SkColor m_Color = SK_ColorBLACK;
		Gradient m_Gradient;

		SkColor m_BorderColor = SK_ColorBLACK;
		Gradient m_BorderGradient;
		SkPaint::Style m_PaintStyle = SkPaint::Style::kFill_Style;
		uint32_t m_BorderWidth = 0;

		uint32_t m_Width = 50;
		uint32_t m_Height = 50;
		
		uint32_t m_BottomOffset = 0;
		uint32_t m_RightOffset = 0;
		
		AABB m_Bounds;
		Window& m_Window;
		
		Vector2 m_Alignment = Vector2(0.f);
		Anchor m_Anchor = E_ANCH_TOP_LEFT;
		
		bool m_ShowFill = true;
		bool m_UseAntialiasing = true;
		bool m_ScaleWithDPI = true;
		
		Vector4 m_Padding;

		virtual void BeginDraw() override;
		virtual void Draw() override = 0;
		virtual void EndDraw() override;

	private:
		Vector2 ApplyAnchor(const Vector2& ScreenPosition, const Vector2& ScreenScale);
		Vector2 ApplyAlignment(const Vector2& ScreenScale) const;

		UILayout* CheckParentLayout(Entity& Origin) const;
	};
}
