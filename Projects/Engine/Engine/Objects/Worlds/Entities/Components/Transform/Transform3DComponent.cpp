#include "pch.h"
#include "Transform3DComponent.h"
#include "Transform2DComponent.h"

namespace Engine
{
	Transform3DComponent::Transform3DComponent(Entity& Entity, const std::string& sName) : TransformComponent<Vector3>(Entity, sName)
	{
		AddProhibitedTypes<Transform2DComponent>();
	}
	
	const Vector3& Transform3DComponent::GetRotation(bool Local) const
	{
		if (Local)
		{
			return m_Rotation;
		}

		// TODO: Global
		return m_Rotation;
	}

	void Transform3DComponent::SetRotation(const Vector3& Rotation, bool Local)
	{
		if (m_Rotation == Rotation)
			return;

		MarkDirty(E_ROTATION);
		
		if (Local)
		{
			m_Rotation = Rotation;
			return;
		}

		// TODO: Global
		m_Rotation = Rotation;
	}

	void Transform3DComponent::Rotate(const Vector3& Rotation)
	{
		if (Rotation == Vector3(0.f))
			return;
		
		m_Rotation += Rotation;
		MarkDirty(E_ROTATION);
	}
}
