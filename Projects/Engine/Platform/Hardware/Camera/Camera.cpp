#include "pch.h"
#include "Camera.h"

namespace Engine
{
	int32_t Camera::m_Orientation = 0.f;

	Camera::~Camera()
	{
		Stop();
	}

	bool Camera::Start(CameraType Type)
	{	
		if (m_bStarted)
		{
			if (Type != m_Type)
				Stop();
			else
				return false;
		}

		m_Type = Type;

		Open();
		m_bStarted = true;

		return true;
	}

	bool Camera::Stop()
	{
		if (!m_bStarted)
			return false;

		Close();
		m_bStarted = false;
		
		return true;
	}

	bool Camera::Open()
	{
		if (m_bOpened)
			return false;

		m_bOpened = true;
		
		return true;
	}

	bool Camera::Close()
	{
		if (!m_bOpened)
			return false;

		m_bOpened = false;
		
		return true;
	}
}