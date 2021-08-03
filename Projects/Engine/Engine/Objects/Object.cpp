#include "pch.h"
#include "Object.h"

namespace Engine
{
	Object::Object(const String& sName)
	{
		m_sName = sName;
		
		CB_CORE_TRACE("Created object {0}", m_sName);
	}

	Object::~Object()
	{
		CB_CORE_TRACE("Destroyed object {0}", m_sName);
	}
}
