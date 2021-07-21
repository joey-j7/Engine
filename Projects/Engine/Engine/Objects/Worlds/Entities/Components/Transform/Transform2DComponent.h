#pragma once

#include "TransformComponent.h"

namespace Engine
{
	class Engine_API Transform2DComponent : public TransformComponent<Vector2>
	{
	public:
		Transform2DComponent(Entity& Entity, const std::string& sName = "Transform 2D Component");

		float GetAngle(bool Local = true) const;
		void SetAngle(float Angle, bool Local = true);
		void Rotate(float Angle);

	protected:
		float m_Angle = 0.f;
	};
}
