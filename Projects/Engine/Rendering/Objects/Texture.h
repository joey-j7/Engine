#pragma once

#include "Engine/Objects/Object.h"

namespace Engine
{
	class Engine_API Texture : public Object
	{
	public:
		Texture(const std::string& sName = "Unnamed Texture") : Object(sName) {};
		virtual ~Texture() = default;
	};
}