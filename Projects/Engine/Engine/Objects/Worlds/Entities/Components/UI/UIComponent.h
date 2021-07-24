#pragma once

#include <map>

#include "Engine/Objects/Worlds/Entities/Components/RenderComponent.h"

#include <include/core/SkCanvas.h>

namespace Engine
{
	class Renderer2D;
	
	class Engine_API UIComponent : public RenderComponent
	{
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
		
		UIComponent(Entity& Entity, const std::string& sName = "UI Component");
		virtual ~UIComponent();

		const Vector2& GetPosition() const;
		float GetRotation() const;
		const Vector2& GetScale() const;

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

		bool GetAntialiasing() const { return m_UseAntialiasing; }
		virtual void SetAntialiasing(bool AA);

		virtual const AABB GetBounds() const override;

	protected:
		SkCanvas* m_Canvas = nullptr;
		SkPaint m_Paint;

		SkColor m_Color = SK_ColorBLACK;
		Gradient m_Gradient;

		SkColor m_BorderColor = SK_ColorBLACK;
		Gradient m_BorderGradient;
		SkPaint::Style m_PaintStyle = SkPaint::Style::kFill_Style;
		uint32_t m_BorderWidth = 0;

		uint32_t m_Width = 50;
		uint32_t m_Height = 50;

		AABB m_Bounds;
		
		bool m_ShowFill = true;
		bool m_UseAntialiasing = true;
		
		Vector4 Padding;

	private:
		virtual void BeginDraw() override;
		virtual void Draw() override = 0;
		virtual void EndDraw() override;
	};
}
