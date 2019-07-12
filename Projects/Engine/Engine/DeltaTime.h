#pragma once

namespace Engine
{
	class DeltaTime
	{
	public:
		static float Get() { return m_DeltaTime; }
		static float GetFixed() { return m_FixedTimeStep; }

		static bool IsFixed() { return m_bIsFixed; }

		void Update();

	private:
		static bool m_bIsFixed;
		static const float m_FixedTimeStep;
		static double m_FixedTime;

		static double m_CurrentTime;
		static double m_LastTime;
		static float m_DeltaTime;
	};
}