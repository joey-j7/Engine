#pragma once

#include "Engine/General/Collision.h"

namespace Engine
{
	class Renderer;

	class Engine_API Renderable
	{
		friend class Renderer;
		friend class Renderer2D;
		friend class Renderer3D;

	public:
		Renderable();
		~Renderable();

		virtual const AABB GetBounds() = 0;

	protected:
		void MarkDirty();

		enum Dimension
		{
			E_NONE = 0,
			E_2D = 1,
			E_3D = 2
		};

		Dimension m_RenderDimension = E_3D;
		Renderer* m_Renderer = nullptr;

	private:
		virtual void BeginDraw() = 0;
		virtual void Draw() = 0;
		virtual void EndDraw() = 0;

		enum State
		{
			E_IDLE = 0,
			E_MODIFIED = 1
		};

		State m_RenderState = E_IDLE;
	};
}
