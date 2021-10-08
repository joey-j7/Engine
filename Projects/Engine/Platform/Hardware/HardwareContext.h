#pragma once

#include "Camera/Camera.h"

#ifdef CB_PLATFORM_ANDROID
#include "Platform/Android/AndPermissionManager.h"
#else
#include "Platform/PermissionManager.h"
#endif

namespace Engine
{
	class Engine_API HardwareContext
	{
	public:
		HardwareContext();

		Camera& GetCamera();

		void Pause();
		void Resume();

	private:
		std::unique_ptr<Camera> m_Camera;
		std::unique_ptr<PermissionManager> m_PermissionManager;
	};
}
