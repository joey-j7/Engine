#pragma once

#include "Engine/Application.h"

#include "Engine/Objects/Renderable.h"
#include "Transform/Transform.h"

#include "Transform/Transform2DComponent.h"
#include "Transform/Transform3DComponent.h"

namespace Engine
{
	template <typename T, typename R, typename S>
	class Engine_API RenderComponent : public Component, public Renderable
	{
		static_assert(
			(!std::is_base_of<Vector2, T>::value && !std::is_base_of<Vector3, T>::value) ||
			(std::is_base_of<Vector2, T>::value && (!std::is_base_of<Transform2DComponent, R>::value || !std::is_base_of<float, S>::value)) ||
			(std::is_base_of<Vector3, T>::value && (!std::is_base_of<Transform3DComponent, R>::value || !std::is_base_of<Vector3, S>::value)),
			"Invalid template type detected"
		);
		
	public:
		RenderComponent(Entity& Entity, const String& sName = "Button") :
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
			
			GetDependency<R>()->OnTransformation.Bind(
				this, &RenderComponent<T, R, S>::OnTransformation
			);

			MarkDirty();
		}

		const T& GetPosition(bool Local = false) const
		{
			return GetDependency<R>()->GetPosition(Local);
		}

		void SetPosition(const T& Position)
		{
			return GetDependency<R>()->SetPosition(Position);
		}
		
		const S& GetRotation(bool Local = false) const
		{
			return GetDependency<R>()->GetRotation(Local);
		}

		void SetRotation(const R& Rotation)
		{
			return GetDependency<R>()->SetRotation(Rotation);
		}
		
		const T& GetScale(bool Local = false) const
		{
			return GetDependency<R>()->GetScale(false);
		}

		void SetScale(const T& Scale)
		{
			return GetDependency<R>()->SetScale(Scale);
		}

		R* GetTransform() const
		{
			return GetDependency<R>();
		}

	private:
		void OnTransformation(const Transform<T, S>& Old, const Transform<T, S>& New)
		{
			MarkDirty();
		}
	};

	typedef RenderComponent<Vector3, Transform3DComponent, Vector3> Render3DComponent;
	typedef RenderComponent<Vector2, Transform2DComponent, float> Render2DComponent;
}
