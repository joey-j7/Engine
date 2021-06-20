#pragma once

#ifdef CB_PLATFORM_WINDOWS

#include "Engine/Core.h"
#include "Platform/Hardware/Camera/Camera.h"

namespace Engine
{
	// Interface representing a desktop system based Window
	class Engine_API WinCamera : public Camera
	{
	public:
		WinCamera() {}
		virtual ~WinCamera() {};
	};
}

#endif