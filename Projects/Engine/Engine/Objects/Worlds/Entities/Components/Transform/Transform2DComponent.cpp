#include "pch.h"
#include "Transform2DComponent.h"
#include "Transform3DComponent.h"

namespace Engine
{
	Transform2DComponent::Transform2DComponent(Entity& Entity, const std::string& sName) : TransformComponent<Vector2>(Entity, sName)
	{
		AddDependencyTypes<Transform3DComponent, Transform3DComponent>();
		AddProhibitedTypes<Transform3DComponent, Transform3DComponent>();
	}
	
	float Transform2DComponent::GetAngle(bool Local) const
	{
		if (Local)
		{
			return m_Angle;
		}

		// TODO: Global
		return m_Angle;
	}

	void Transform2DComponent::SetAngle(float Angle, bool Local)
	{
		if (m_Angle == Angle)
			return;
		
		MarkDirty(E_ROTATION);
		
		if (Local)
		{
			m_Angle = Angle;
			return;
		}

		// TODO: Global
		m_Angle = Angle;
	}

	void Transform2DComponent::Rotate(float Angle)
	{
		if (Angle == 0.f)
			return;
		
		m_Angle += Angle;
		MarkDirty(E_ROTATION);
	}
}
