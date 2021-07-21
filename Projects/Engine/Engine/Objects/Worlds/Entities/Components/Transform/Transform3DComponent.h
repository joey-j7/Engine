#pragma once

#include "TransformComponent.h"

namespace Engine
{
	class Engine_API Transform3DComponent : public TransformComponent<Vector3>
	{
	public:
		Transform3DComponent(Entity& Entity, const std::string& sName = "Transform 3D Component");

		
		const Vector3& GetRotation(bool Local = true) const;
		void SetRotation(const Vector3& Rotation, bool Local = true);
		void Rotate(const Vector3& Rotation);

	protected:
		Vector3 m_Rotation;
	};
}
