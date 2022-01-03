#include "pch.h"
#include "Stateable.h"

namespace Engine
{
	bool Stateable::Play()
	{
		if (Resume())
			return true;

		if (m_State != STOPPED)
			return false;

		m_State = ACTIVE;

		return true;
	}

	bool Stateable::Pause()
	{
		if (m_State != ACTIVE)
			return false;

		m_State = PAUSED;

		return true;
	}

	bool Stateable::Resume()
	{
		if (m_State == ACTIVE || m_State == STOPPED)
			return false;

		m_State = ACTIVE;
		return true;
	}

	bool Stateable::Stop()
	{
		if (m_State == STOPPED)
			return false;

		m_State = STOPPED;
		return true;
	}
}
