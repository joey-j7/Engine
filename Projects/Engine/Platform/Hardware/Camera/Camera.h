#pragma once

#include "Engine/Core.h"

#include <include/core/SkImage.h>

namespace Engine
{
	enum CameraType
	{
		E_CAMERA_BACKFACE = 0,
		E_CAMERA_FRONTFACE = 1,
		E_CAMERA_EXTERNAL = 2
	};

	class UIImage;
	
	// Interface representing a desktop system based Window
	class Engine_API Camera
	{
	public:
		Camera() {}
		virtual ~Camera();

		virtual bool Start(CameraType Type = E_CAMERA_BACKFACE);
		virtual bool Stop();

		virtual bool Pause();
		virtual bool Resume();

		virtual bool IsReady() { return true; };

		virtual void TakePhoto() = 0;
		const String& GetLastPhotoPath() const { return m_LastPhotoPath; }

		static int32_t GetOrientation() { return m_Orientation; }

		UIImage* GetPreviewImage()
		{
			return m_PreviewImage;
		}

		void SetPreviewImage(UIImage* Image)
		{
			m_PreviewImage = Image;
		}

		bool IsStarting() const { return m_bStarting; }
		bool IsStarted() const { return m_bStarted; }
		bool IsOpened() const { return m_bOpened; }
		
		Event<void> OnStartCallback = Event<void>("Camera::OnStartCallback");
		Event<void, const String&> OnPhotoTakenCallback = Event<void, const String&>("Camera::OnPhotoTakenCallback");
		
	protected:
		virtual bool Open();
		virtual bool Close();

		static bool m_bStarting;

		bool m_bStarted = false;
		bool m_bOpened = false;
		bool m_bPaused = false;

		static int32_t m_Orientation;
		String m_LastPhotoPath = "";

		CameraType m_Type = E_CAMERA_FRONTFACE;

		UIImage* m_PreviewImage = nullptr;
	};

}
