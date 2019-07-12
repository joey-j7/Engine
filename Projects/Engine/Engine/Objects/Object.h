#pragma once

#include <string>

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