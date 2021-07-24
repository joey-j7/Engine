#pragma once

#include "Engine/General/Math.h"

namespace Engine
{
	template <typename T = Vector3, typename R = Vector3>
	struct Transform
	{
		static_assert(std::is_same<T, Vector2>() || std::is_same<T, Vector3>(), "Illegal transform type detected");
		static_assert(std::is_same<R, float>() || std::is_same<R, Vector3>(), "Illegal rotation type detected");

		Transform() {}
		Transform(const T& Position, const R& Rotation, const T& Scale) : m_Position(Position),
			m_Rotation(Rotation),
			m_Scale(Scale)
		{}

		T m_Position = T(0.f);
		R m_Rotation = R(0.f);
		T m_Scale = T(1.f);
	};
}