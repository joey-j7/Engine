#include "pch.h"
#include "RenderComponent.h"

#include "Engine/Application.h"

#include "Transform/TransformComponent2D.h"
#include "Transform/TransformComponent3D.h"

namespace Engine
{
	RenderComponent::RenderComponent(Entity& Entity, const std::string& sName) : Component(Entity, sName)
	{
		GetEntity().OnComponentAdded.Bind(this, &RenderComponent::OnComponentAdded);
		GetEntity().OnComponentRemoved.Bind(this, &RenderComponent::OnComponentRemoved);

		// Configure renderers
		if (const auto Transform3D = GetEntity().GetComponent<TransformComponent3D>())
			OnComponentAdded(*Transform3D);
		else if (const auto Transform2D = GetEntity().GetComponent<TransformComponent2D>())
			OnComponentAdded(*Transform2D);
		else
			AddDependencyTypes<TransformComponent3D>();

		m_Renderer = Application::Get().GetRenderContext().GetAPI().GetRenderer2D();
	}

	void RenderComponent::OnComponentAdded(Component& Component)
	{
		if (Component.GetID() == GetClassID<TransformComponent3D>())
		{
			static_cast<TransformComponent3D*>(&Component)->OnTransformation.Bind(
				this, &RenderComponent::OnTransformation<Vector3, Vector3>
			);
			
			m_RenderDimension = E_3D;
			m_Renderer = Application::Get().GetRenderContext().GetAPI().GetRenderer3D();

			MarkDirty();
		}
		else if (Component.GetID() == GetClassID<TransformComponent2D>())
		{
			static_cast<TransformComponent2D*>(&Component)->OnTransformation.Bind(
				this, &RenderComponent::OnTransformation<Vector2, float>
			);
			
			m_RenderDimension = E_2D;
			m_Renderer = Application::Get().GetRenderContext().GetAPI().GetRenderer2D();

			MarkDirty();
		}
	}

	void RenderComponent::OnComponentRemoved(Component& Component)
	{
		if (m_RenderDimension == E_3D && Component.GetID() == GetClassID<TransformComponent3D>())
		{
			static_cast<TransformComponent3D*>(&Component)->OnTransformation.Unbind(
				this, &RenderComponent::OnTransformation
			);
			
			m_RenderDimension = E_NONE;
			m_Renderer = nullptr;
		}
		else if (m_RenderDimension == E_2D && Component.GetID() == GetClassID<TransformComponent2D>())
		{
			static_cast<TransformComponent2D*>(&Component)->OnTransformation.Unbind(
				this, &RenderComponent::OnTransformation
			);
			
			m_RenderDimension = E_NONE;
			m_Renderer = nullptr;
		}
	}
}
