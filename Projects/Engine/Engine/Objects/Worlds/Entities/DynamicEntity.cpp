#include "pch.h"
#include "DynamicEntity.h"

namespace Engine
{
	DynamicEntity::DynamicEntity(const String& sName) : DynamicObject(sName), Entity()
	{
		m_Type = E_DYNAMIC;
	}

	DynamicEntity::~DynamicEntity()
	{
		OnDestroy();
	}
}
