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
	HardwareContext::HardwareContext()
	{
#ifdef CB_PLATFORM_ANDROID
		m_PermissionManager = std::unique_ptr<AndPermissionManager>(
			new AndPermissionManager()
		);
#else
		m_PermissionManager = std::unique_ptr<PermissionManager>(
			new PermissionManager()
		);
#endif
	}

	Camera& HardwareContext::GetCamera()
	{
		if (!m_Camera)
		{
#ifdef CB_PLATFORM_ANDROID
			m_Camera = std::unique_ptr<AndCamera>(
				new AndCamera()
			);
#else
			m_Camera = std::unique_ptr<WinCamera>(
				new WinCamera()
			);
#endif
		}

		return *m_Camera.get();
	}

	void HardwareContext::Pause()
	{
		if (m_Camera.get())
			m_Camera->Pause();
	}

	void HardwareContext::Resume()
	{
		if (m_Camera.get())
			m_Camera->Resume();
	}
}