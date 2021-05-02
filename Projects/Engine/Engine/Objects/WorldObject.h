#pragma once

#include "Object.h"

namespace Engine
{
	class Engine_API WorldObject : public Object
	{
	public:
		WorldObject(const std::string& sName = "Unnamed Object") : Object(sName) {};

	};
}
