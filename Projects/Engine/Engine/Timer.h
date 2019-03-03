#pragma once

#include <functional>
#include <cmath>

#include "Engine/DeltaTime.h"

//template<class ... Types>
class Timer
{
public:
	Timer(float start, bool loop = true) : m_Start(start), m_Current(start), m_Loop(loop) {};

	void Init(float start, bool loop = true) {
		m_Start = start;
		m_Current = start;
		m_Loop = loop;
	};
	
	bool Run()
	{
		m_Current = (m_Current > 0.0f) ? (m_Current - DeltaTime::Get()) : (m_Loop ? (std::fmod(m_Current, m_Start) + m_Start) : 0.0f);
		return m_Current <= 0.0f;
	}

private:
	float m_Start = 1.0f;
	float m_Current = 1.0f;

	bool m_Loop = true;

	// std::function<Types ...> m_Function;
};