#pragma once

#include "Engine/Core.h"
#include <string>

class Component;

namespace Engine
{
	class Engine_API Object
	{
	public:
		Object(const std::string& sName = "Unnamed Object");

		const std::string& GetName() const { return m_sName; }

	protected:
		std::string m_sName = "";
	};
}
