#include "pch.h"
#include "VkWrapper.h"

namespace Engine
{
	VkWrapper::VkWrapper(const std::string& sName) : Object(sName), m_API(VkRenderAPI::Get())
	{

	}
}