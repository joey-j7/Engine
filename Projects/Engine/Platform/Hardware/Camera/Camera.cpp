#include "pch.h"
#include "Camera.h"

namespace Engine
{
	int32_t Camera::m_Orientation = 0.f;
	bool Camera::m_bStarting = false;

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

	bool Camera::Pause()
	{
		if (!m_bStarted)
			return false;

		Stop();

		m_bPaused = true;
		return true;
	}

	bool Camera::Resume()
	{
		if (!m_bPaused)
			return false;

		Start(m_Type);

		m_bPaused = false;
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