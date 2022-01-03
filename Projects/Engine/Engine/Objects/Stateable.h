#pragma once

#include "Object.h"

namespace Engine
{
	class Engine_API Stateable
	{
	public:
		enum State
		{
			ACTIVE,
			PAUSED,
			STOPPED
		};

		Stateable() {}
		virtual ~Stateable() { };

		virtual bool IsActive() const { return m_State == ACTIVE; }
		virtual bool IsStopped() const { return m_State == STOPPED; }

		virtual State GetState() const { return m_State; }

		virtual bool Play();
		virtual bool Pause();
		virtual bool Stop();

	protected:
		virtual bool Resume();

		State m_State = ACTIVE;
	};
}