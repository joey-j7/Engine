#pragma once

#include "Component.h"

#include "Engine/Objects/Worlds/Entities/Entity.h"
#include "Transform/TransformComponent.h"

namespace Engine
{
	class Renderer;
	
	class Engine_API RenderComponent : public Component
	{
		friend class Renderer;
		friend class Renderer2D;
		friend class Renderer3D;
		
	public:
		RenderComponent(Entity& Entity, const std::string& sName = "Button");

	protected:
		void MarkDirty();
		
		virtual void Draw() = 0;
		
		enum Dimension
		{
			E_NONE = 0,
			E_2D = 1,
			E_3D = 2
		};
		
		Dimension m_RenderDimension = E_3D;
		Renderer* m_Renderer = nullptr;

	private:
		void OnComponentAdded(Component& Component);
		void OnComponentRemoved(Component& Component);

		template<typename T, typename R>
		void OnTransformation(const Transform<T, R>& Old, const Transform<T, R>& New)
		{
			MarkDirty();
		}

		virtual void BeginDraw() = 0;
		virtual void EndDraw() = 0;
		
		enum State
		{
			E_IDLE = 0,
			E_MODIFIED = 1
		};

		State m_RenderState = E_IDLE;
	};
}
