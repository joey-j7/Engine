#include "pch.h"
#include "DeltaTime.h"

void DeltaTime::Update()
{
	m_CurrentTime = glfwGetTime();
	m_DeltaTime = static_cast<float>(m_CurrentTime - m_LastTime);
	m_LastTime = m_CurrentTime;
}

double DeltaTime::m_CurrentTime = 0;
double DeltaTime::m_LastTime = 0;
float DeltaTime::m_DeltaTime = 0.0f;