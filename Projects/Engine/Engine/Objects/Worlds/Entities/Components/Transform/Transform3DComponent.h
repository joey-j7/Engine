#pragma once

#include "TransformComponent.h"

namespace Engine
{
	class Engine_API Transform3DComponent : public TransformComponent<glm::vec3>
	{
	public:
		Transform3DComponent(Entity& Entity, const std::string& sName = "Transform 3D Component") : TransformComponent<glm::vec3>(Entity, sName) {};

		const glm::vec3& GetRotation() const;
		void SetRotation(const glm::vec3& Rotation);
		void Rotate(const glm::vec3& Rotation, bool Local = true);

	protected:
		glm::vec3 m_Rotation;
	};
}
