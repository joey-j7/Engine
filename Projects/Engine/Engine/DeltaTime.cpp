#include "pch.h"
#include "DeltaTime.h"

namespace Engine
{
	void DeltaTime::Update()
	{
		m_LastTime = m_CurrentTime;
		m_CurrentTime = glfwGetTime();
		m_DeltaTime = static_cast<float>(m_CurrentTime - m_LastTime);

		m_FixedTime += m_CurrentTime - m_LastTime;
		m_bIsFixed = m_FixedTime >= m_FixedTimeStep;

		m_FrameCounter++;
		m_FrameTimer += m_DeltaTime;

		if (m_FrameTimer >= 1.f)
		{
			m_FPS = m_FrameCounter;

			m_FrameTimer = std::fmod(m_FrameTimer, 1.f);
			m_FrameCounter = 0;
		}

		if (m_bIsFixed)
			m_FixedTime = std::fmod(m_FixedTime, static_cast<double>(m_FixedTimeStep));
	}

	void DeltaTime::Reset()
	{
		m_CurrentTime = glfwGetTime();
		m_LastTime = m_CurrentTime;

		m_FrameTimer = 0;
		m_FrameCounter = 0;
	}

	bool DeltaTime::m_bIsFixed = false;
	double DeltaTime::m_FixedTime = 0;
	const float DeltaTime::m_FixedTimeStep = 0.2f;

	double DeltaTime::m_CurrentTime = 0;
	double DeltaTime::m_LastTime = 0;
	float DeltaTime::m_DeltaTime = 0.0f;

	uint32_t DeltaTime::m_FPS = 60;

	float DeltaTime::m_FrameTimer = 0.f;
	uint32_t DeltaTime::m_FrameCounter = 0;
}