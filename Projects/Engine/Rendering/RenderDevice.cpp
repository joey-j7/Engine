#include "pch.h"
#include "RenderDevice.h"

namespace Engine
{
	RenderDevice::~RenderDevice()
	{
		Free();
	}

	void RenderDevice::Free()
	{
		for (Shader* shader : m_Shaders)
		{
			delete shader;
		}

		m_Shaders.clear();
	}
}