#pragma once

#ifdef CB_PLATFORM_ANDROID

#include "Engine/Core.h"
#include "Platform/Hardware/Camera/Camera.h"

#include <include/core/SkImage.h>
#include <include/core/SkPicture.h>

#include <camera/NdkCameraDevice.h>
#include <camera/NdkCameraCaptureSession.h>
#include <camera/NdkCameraManager.h>

#include <media/NdkImageReader.h>
#include <media/NdkImage.h>

#include <memory.h>
#include <mutex>

namespace Engine
{
	// Interface representing a desktop system based Window
	class Engine_API AndCamera : public Camera
	{
	public:
		enum CaptureState
		{
			E_CLOSED = 0,
			E_INITIALIZING = 1,
			E_READY = 2,
			E_ACTIVE = 3
		};

		AndCamera();
		virtual ~AndCamera();

		virtual bool Start(CameraType Type) override;
		virtual bool Stop() override;

		virtual bool IsReady() override;

		void TakePhoto() override;

		// TODO: Move to platform context
		static void ResetReader();

		static bool IsCapturing() {
			return m_IsCapturing;
		}

	protected:
		void StartPreview();
		void StopCapture();
		
		void OnPermission(const std::vector<String>& Permissions, const std::vector<int32_t>& Granted);
		void OnPhotoProcessed();
		
		virtual bool Open() override;
		virtual bool Close() override;

		String PollID();

		static void OnDisconnected(void* Context, ACameraDevice* Device);
		static void OnError(void* Context, ACameraDevice* Device, int Error);
		
		static void OnReady(void* Context, ACameraCaptureSession* Session);
		static void OnActive(void* Context, ACameraCaptureSession* Session);
		static void OnClosed(void* Context, ACameraCaptureSession* Session);

		static void OnCaptureBufferLost(void* context, ACameraCaptureSession* session, ACaptureRequest* request, ACameraWindowType* window, int64_t frameNumber);
		static void OnCaptureCompleted(void* context, ACameraCaptureSession* session, ACaptureRequest* request, const ACameraMetadata* result);
		static void OnCaptureFailed(void* context, ACameraCaptureSession* session, ACaptureRequest* request, ACameraCaptureFailure* failure);
		static void OnCaptureProgressed(void* context, ACameraCaptureSession* session, ACaptureRequest* request, const ACameraMetadata* result);
		static void OnCaptureSequenceAborted(void* context, ACameraCaptureSession* session, int sequenceId);
		static void OnCaptureSequenceCompleted(void* context, ACameraCaptureSession* session, int sequenceId, int64_t frameNumber);
		static void OnCaptureStarted(void* context, ACameraCaptureSession* session, const ACaptureRequest* request, int64_t timestamp);

		static void OnPreviewRetrieved(void* context, AImageReader* reader);
		static void OnPhotoRetrieved(void* context, AImageReader*);
		
		ANativeWindow* m_PreviewWindow = nullptr;
		
		ACameraManager* m_Manager = nullptr;
		ACameraDevice* m_Device = nullptr;
		ACameraOutputTarget* m_OutputTarget = nullptr;
		ACaptureRequest* m_CaptureRequest = nullptr;
		ACameraCaptureSession* m_CaptureSession = nullptr;
		ACaptureSessionOutput* m_SessionOutput = nullptr;
		ACaptureSessionOutput* m_TextureOutput = nullptr;
		ACaptureSessionOutputContainer* m_CaptureSessionOutputContainer = nullptr;
		ACameraMetadata* m_Metadata = nullptr;
		static AImageReader* m_Reader;
		
		ACameraDevice_StateCallbacks m_DeviceStateCallbacks;
		ACameraCaptureSession_stateCallbacks m_CaptureSessionStateCallbacks;
		ACameraCaptureSession_captureCallbacks m_CaptureSessionCaptureCallbacks;

		static AImageReader_ImageListener m_PreviewImageCallbacks;
		static AImageReader_ImageListener m_PhotoImageCallbacks;
		
		String m_ID = "";

		static CaptureState m_CaptureState;

		static bool m_IsCapturing;
		static bool m_IsTakingPhoto;

		static std::mutex m_Mutex;

		bool m_HasPermission = false;
	};
}

#endif