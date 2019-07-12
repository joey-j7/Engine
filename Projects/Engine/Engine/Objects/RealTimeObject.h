#pragma once

#include "Object.h"

namespace Engine
{
	class Engine_API RealTimeObject : public Object
	{
	public:
		enum State
		{
			PLAYING,
			PAUSED,
			STOPPED
		};

		RealTimeObject(const std::string& sName = "Unnamed Object") : Object(sName) {}
		virtual ~RealTimeObject() { Destroy(); };

		bool IsActive() const { return m_State == PLAYING; }
		bool IsStopped() const { return m_State == STOPPED; }

		State GetState() const { return m_State; }

		virtual bool Play();
		virtual bool Pause();
		virtual bool Stop();

	protected:
		virtual void Update(float fDeltaTime) = 0;
		virtual void FixedUpdate(float fDeltaTime) = 0;

		virtual void Draw(float fDeltaTime) = 0;
		virtual void LateUpdate(float fDeltaTime) = 0;

		virtual bool Resume();

		virtual bool Destroy();

		State m_State = PLAYING;
	};
}