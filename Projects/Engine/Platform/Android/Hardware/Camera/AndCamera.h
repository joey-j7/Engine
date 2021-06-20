#pragma once

#ifdef CB_PLATFORM_ANDROID

#include "Engine/Core.h"
#include "Platform/Hardware/Camera/Camera.h"

#include <camera/NdkCameraDevice.h>
#include <camera/NdkCameraCaptureSession.h>
#include <camera/NdkCameraManager.h>

#include <memory.h>

namespace Engine
{
	// Interface representing a desktop system based Window
	class Engine_API AndCamera : public Camera
	{
	public:
		AndCamera() {}
		virtual ~AndCamera();

		virtual bool Start(CameraType Type) override;
		virtual bool Stop() override;

		void OnCameraPermission(jboolean Granted);
		
	protected:
		void InternalStart();
		
		virtual bool Open() override;
		virtual bool Close() override;

		std::string PollID();

		static void OnDisconnected(void* Context, ACameraDevice* Device);
		static void OnError(void* Context, ACameraDevice* Device, int Error);
		
		static void OnReady(void* Context, ACameraCaptureSession* Session);
		static void OnActive(void* Context, ACameraCaptureSession* Session);
		static void OnClosed(void* Context, ACameraCaptureSession* Session);

		// TODO: Move to platform context
		static bool RequestPermission();

		// ANativeWindow* m_TextureWindow = nullptr;
		
		ACameraManager* m_Manager = nullptr;
		ACameraDevice* m_Device = nullptr;
		ACameraOutputTarget* m_OutputTarget = nullptr;
		ACaptureRequest* m_CaptureRequest = nullptr;
		ACameraCaptureSession* m_CaptureSession = nullptr;
		ACaptureSessionOutput* m_SessionOutput = nullptr;
		ACaptureSessionOutput* m_TextureOutput = nullptr;
		ACaptureSessionOutputContainer* m_CaptureSessionOutputContainer = nullptr;
		ACameraMetadata* m_Metadata = nullptr;
		
		ACameraDevice_StateCallbacks m_DeviceStateCallbacks;
		ACameraCaptureSession_stateCallbacks m_CaptureSessionStateCallbacks;
		ACameraCaptureSession_captureCallbacks m_CaptureSessionCaptureCallbacks;

		std::string m_ID = "";
		bool HasPermission = false;
	};
}

#endif