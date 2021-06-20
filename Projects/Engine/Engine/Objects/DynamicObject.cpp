#include "pch.h"
#include "DynamicObject.h"

namespace Engine
{
	bool DynamicObject::Play()
	{
		if (Resume())
			return true;

		if (m_State != STOPPED)
			return false;

		m_State = ACTIVE;

		return true;
	}

	bool DynamicObject::Pause()
	{
		if (m_State != ACTIVE)
			return false;

		m_State = PAUSED;

		return true;
	}

	bool DynamicObject::Resume()
	{
		if (m_State == ACTIVE || m_State == STOPPED)
			return false;

		m_State = ACTIVE;
		return true;
	}

	bool DynamicObject::Stop()
	{
		if (m_State == STOPPED)
			return false;

		m_State = STOPPED;
		return true;
	}
}
