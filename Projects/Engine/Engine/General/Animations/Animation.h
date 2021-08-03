#pragma once

#include "Engine/Objects/DynamicObject.h"

namespace Engine
{
	class Engine_API Animation : public DynamicObject
	{
	public:
		Animation(
			const String& sName = "Animation"
		) : DynamicObject(sName) {}

	private:
		virtual void Update(float fDeltaTime) = 0;

		void FixedUpdate(float fDeltaTime) final {}

		void Draw(float fDeltaTime) final {}
		void LateUpdate(float fDeltaTime) final {}

		uint32_t m_Length = 0;
	};
}