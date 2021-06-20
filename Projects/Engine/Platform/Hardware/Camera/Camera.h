#pragma once

#include "Engine/Core.h"

namespace Engine
{
	enum CameraType
	{
		E_CAMERA_BACKFACE = 0,
		E_CAMERA_FRONTFACE = 1
	};
	
	// Interface representing a desktop system based Window
	class Engine_API Camera
	{
	public:
		Camera() {}
		virtual ~Camera();

		virtual bool Start(CameraType Type = E_CAMERA_FRONTFACE);
		virtual bool Stop();

	protected:
		virtual bool Open();
		virtual bool Close();

		bool m_bStarted = false;
		bool m_bOpened = false;

		CameraType m_Type = E_CAMERA_FRONTFACE;
	};

}