#pragma once

class DeltaTime
{
public:
	static float Get() { return m_DeltaTime; }
	void Update();

private:
	static double m_CurrentTime;
	static double m_LastTime;
	static float m_DeltaTime;
};