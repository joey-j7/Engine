#include "pch.h"
#include "HardwareContext.h"

// TODO: Cleanup
#ifdef CB_PLATFORM_ANDROID
#include "Platform/Android/Hardware/Camera/AndCamera.h"
#else
#include "Platform/Windows/Hardware/Camera/WinCamera.h"
#endif

namespace Engine
{
	Camera& HardwareContext::GetCamera()
	{
		if (!m_pCamera)
		{
#ifdef CB_PLATFORM_ANDROID
			m_pCamera = std::unique_ptr<AndCamera>(
				new AndCamera()
				);
#else
			m_pCamera = std::unique_ptr<WinCamera>(
				new WinCamera()
				);
#endif
		}

		return *m_pCamera.get();
	}

	void HardwareContext::Pause()
	{
		if (m_pCamera.get())
			m_pCamera->Pause();
	}

	void HardwareContext::Resume()
	{
		if (m_pCamera.get())
			m_pCamera->Resume();
	}
}