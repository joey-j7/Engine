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
	
	// Interface representing a desktop system based Window
	class Engine_API Camera
	{
	public:
		Camera() {}
		virtual ~Camera();

		virtual bool Start(CameraType Type = E_CAMERA_BACKFACE);
		virtual bool Stop();

		virtual void TakePhoto() = 0;

		static int32_t GetOrientation() { return m_Orientation; }

		sk_sp<SkImage>* GetPreviewImage()
		{
			return &m_PreviewImage;
		}
		
		Event<void> OnStartCallback = Event<void>("Camera::OnStartCallback");
		Event<void> OnPhotoTakenCallback = Event<void>("Camera::OnPhotoTakenCallback");
		
	protected:
		virtual bool Open();
		virtual bool Close();

		bool m_bStarted = false;
		bool m_bOpened = false;

		static int32_t m_Orientation;

		CameraType m_Type = E_CAMERA_FRONTFACE;

		sk_sp<SkImage> m_PreviewImage;
	};

}