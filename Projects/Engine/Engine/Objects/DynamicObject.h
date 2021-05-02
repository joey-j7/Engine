#pragma once

#include "WorldObject.h"

namespace Engine
{
	class Engine_API DynamicObject : public Object
	{
	public:
		enum State
		{
			PLAYING,
			PAUSED,
			STOPPED
		};

		DynamicObject(const std::string& sName = "Unnamed Object") : Object(sName) {}
		virtual ~DynamicObject() { };

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

		State m_State = PLAYING;
	};
}