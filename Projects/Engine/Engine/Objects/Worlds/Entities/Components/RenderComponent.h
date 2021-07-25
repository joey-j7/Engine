#pragma once

#include "Component.h"

#include "Engine/Application.h"

#include "Engine/Objects/Renderable.h"
#include "Transform/Transform.h"

#include "Transform/TransformComponent2D.h"
#include "Transform/TransformComponent3D.h"

namespace Engine
{
	template <typename T = Vector3, typename R = TransformComponent3D, typename S = Vector3>
	class Engine_API RenderComponent : public Component, public Renderable
	{
		static_assert(
			(!std::is_base_of<Vector2, T>::value && !std::is_base_of<Vector3, T>::value) ||
			(std::is_base_of<Vector2, T>::value && (!std::is_base_of<TransformComponent2D, R>::value || !std::is_base_of<float, S>::value)) ||
			(std::is_base_of<Vector3, T>::value && (!std::is_base_of<TransformComponent3D, R>::value || !std::is_base_of<Vector3, S>::value)),
			"Invalid template type detected"
		);
		
	public:
		RenderComponent(Entity& Entity, const std::string& sName = "Button") :
			Component(Entity, sName)
		{
			AddDependencyTypes<R>();
			
			if constexpr (std::is_same<Vector2, T>::value)
			{
				m_Renderer = Application::Get().GetRenderContext().GetAPI().GetRenderer2D();
				m_RenderDimension = E_2D;
			}
			else
			{
				m_Renderer = Application::Get().GetRenderContext().GetAPI().GetRenderer3D();
				m_RenderDimension = E_3D;
			}

			GetEntity().GetComponent<R>()->OnTransformation.Bind(
				this, &RenderComponent<T, R, S>::OnTransformation
			);

			MarkDirty();
		}

		const T& GetPosition() const
		{
			return m_Entity.GetComponent<R>()->GetPosition();
		}
		
		const S& GetRotation() const
		{
			return m_Entity.GetComponent<R>()->GetRotation();
		}
		
		const T& GetScale() const
		{
			return m_Entity.GetComponent<R>()->GetScale();
		}

	private:
		void OnTransformation(const Transform<T, S>& Old, const Transform<T, S>& New)
		{
			MarkDirty();
		}
	};

	typedef RenderComponent<Vector3, TransformComponent3D, Vector3> Render3DComponent;
	typedef RenderComponent<Vector2, TransformComponent2D, float> Render2DComponent;
}
