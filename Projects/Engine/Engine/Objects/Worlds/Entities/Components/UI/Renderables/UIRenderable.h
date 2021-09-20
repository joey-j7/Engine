#pragma once

#include <map>

#include "Engine/Objects/Worlds/Entities/Components/UI/UIElement.h"
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
	
	class Engine_API UIRenderable : public Render2DComponent, UIElement
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
				const std::map<SkScalar, Color>& PositionsColors,
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
			
			const std::vector<Color>& GetColors() const { return m_Colors; }
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

			std::vector<Color> m_Colors;
			std::vector<SkScalar> m_Positions;
		};
		
		UIRenderable(Entity& Entity, const String& sName = "UI Component");
		virtual ~UIRenderable();

		const Color& GetColor() const { return m_Color; }
		void SetColor(const Color& Color);

		const Gradient& GetGradient() const { return m_Gradient; }
		void SetGradient(const Gradient& Gradient);

		const SkPaint::Style& GetStyle() const { return m_PaintStyle; }
		
		void SetBorder(uint32_t Width, const Color& Color = SK_ColorBLACK, bool ShowFill = true);
		void SetBorder(uint32_t Width, const Gradient& Gradient, bool ShowFill = true);

		uint32_t GetWidth() const { return m_Width; }
		virtual void SetWidth(uint32_t Width);

		uint32_t GetHeight() const { return m_Height; }
		virtual void SetHeight(uint32_t Height);

		const SkMatrix& GetMatrix();

		void SetSize(uint32_t Width, uint32_t Height);

		uint32_t GetBorderWidth() const { return m_BorderWidth; }
		virtual void SetBorderWidth(uint32_t Thickness);
		
		bool ScalesWithDPI() const { return m_ScaleWithDPI; }
		void ScaleWithDPI(bool Scale);

		bool GetAntialiasing() const { return m_UseAntialiasing; }
		virtual void SetAntialiasing(bool AA);

		const Vector2& GetPivot() const { return m_Pivot; }
		void SetPivot(const Vector2& Pivot);

		Anchor GetAnchor() const { return m_Anchor; }
		void SetAnchor(Anchor Anchor);

		SkShader* GetShader();
		void SetShader(const String& Source);

		template <typename T>
		void SetShaderUniform(const String& UniformName, const T Value)
		{
			if (!m_RuntimeShaderInfo.m_Builder)
			{
				CB_CORE_WARN("Cannot set uniform because shader is not present");
				return;
			}

			m_RuntimeShaderInfo.m_Builder->uniform(UniformName.c_str()) = Value;
			m_RuntimeShaderInfo.m_Dirty = true;

			MarkDirty();
		}

		const Vector2& GetMinSize() const { return m_MinSize; }
		float GetMinWidth() const { return m_MinSize.x; }
		float GetMinHeight() const { return m_MinSize.y; }
		void SetMinSize(const Vector2& Min);

		virtual const AABB GetUnscaledBounds();
		virtual const AABB GetBounds() override;

	protected:
		void SetElementShader(sk_sp<SkShader> Shader);

		void OnParentChanged(Entity& Origin, Entity* Old, Entity* New);

		uint32_t m_ParentChangedID = UINT_MAX;

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

		Color m_Color = SK_ColorBLACK;
		Gradient m_Gradient;

		Color m_BorderColor = SK_ColorBLACK;
		Gradient m_BorderGradient;
		SkPaint::Style m_PaintStyle = SkPaint::Style::kFill_Style;
		uint32_t m_BorderWidth = 5;

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		
		AABB m_Bounds;
		Window& m_Window;
		
		Vector2 m_Pivot = Vector2(0.f);
		Anchor m_Anchor = E_ANCH_TOP_LEFT;

		bool m_ShowFill = true;
		bool m_UseAntialiasing = true;
		bool m_ScaleWithDPI = true;
		
		Vector2 m_MinSize;

		virtual void BeginDraw() override;
		virtual void Draw() override = 0;
		virtual void EndDraw() override;

	private:
		Vector2 CalcAnchor(const Vector2& ScreenScale);
		Vector2 CalcPivot(const Vector2& ScreenScale) const;
	};
}
