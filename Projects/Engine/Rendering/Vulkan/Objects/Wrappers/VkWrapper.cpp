#include "pch.h"
#include "VkWrapper.h"

namespace Engine
{
	VkWrapper::VkWrapper(const String& sName) : Object(sName), m_API(VkRenderAPI::Get())
	{

	}
}