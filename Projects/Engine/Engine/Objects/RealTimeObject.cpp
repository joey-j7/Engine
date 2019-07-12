#include "pch.h"
#include "RealTimeObject.h"

namespace Engine
{
	bool RealTimeObject::Play()
	{
		if (Resume())
			return true;

		if (m_State != STOPPED)
			return false;

		m_State = PLAYING;

		return true;
	}

	bool RealTimeObject::Pause()
	{
		if (m_State != PLAYING)
			return false;

		m_State = PAUSED;

		return true;
	}

	bool RealTimeObject::Resume()
	{
		if (m_State == PLAYING || m_State == STOPPED)
			return false;

		m_State = PLAYING;
		return true;
	}

	bool RealTimeObject::Stop()
	{
		if (m_State == STOPPED)
			return false;

		m_State = STOPPED;
		return true;
	}

	bool RealTimeObject::Destroy()
	{
		return Stop();
	}
}
