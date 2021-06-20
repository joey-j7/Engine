#include "pch.h"
#include "AndCamera.h"

#include "GLFW/glfw3native.h"

#ifdef CB_PLATFORM_ANDROID

#include <android_native_app_glue.h>
#include <memory.h>

#include "Engine/Application.h"

#include <jni.h>
#include <android/log.h>
#define LOG_TAG "libNative"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C"
{
    JNIEXPORT void JNICALL Java_com_appuil_Launcher_NativeLibrary_notifyCameraPermission(
        JNIEnv* env, jclass type, jboolean permission) {
        std::thread permissionHandler(
            &Engine::AndCamera::OnCameraPermission,
            (Engine::AndCamera*)&Engine::Application::Get().GetHardwareContext().GetCamera(),
            permission
        );
        permissionHandler.detach();
    }
};

namespace Engine
{
	AndCamera::~AndCamera()
	{
        Close();
	}

    bool AndCamera::Start(CameraType Type)
    {
        if (m_bStarted)
        {
            if (Type != m_Type)
                Stop();
            else
                return false;
        }
		
        if (!RequestPermission())
            return false;

        m_Type = Type;

		// Permission thread will call once it has received the permission status
        return true;
    }

    bool AndCamera::Stop()
    {
        if (!Camera::Stop())
            return false;

        return true;
    }

	void AndCamera::InternalStart()
	{
        if (!Camera::Start(m_Type))
            return;

        // Prepare surface
        // ANativeWindow* Window = ANativeWindow_fromSurface(env, surface);
        
        android_app* App = glfwGetAndroidApp(
            ((GLFWwindow*)Application::Get().GetRenderContext().GetWindow().GetWindow())
        );
		
        ANativeWindow* Window = App->window;
        camera_status_t Status = ACAMERA_OK;
		
        // Prepare request for texture target
        Status = ACameraDevice_createCaptureRequest(m_Device, TEMPLATE_PREVIEW, &m_CaptureRequest);

        if (Status != ACAMERA_OK)
        {
            CB_CORE_ERROR("Failed to create preview capture request (id: {0}, code: {1})", m_ID, Status);
        }
		
        // Prepare outputs for session
        ACaptureSessionOutput_create(Window, &m_TextureOutput);
        ACaptureSessionOutputContainer_create(&m_CaptureSessionOutputContainer);
        ACaptureSessionOutputContainer_add(m_CaptureSessionOutputContainer, m_TextureOutput);

        // Prepare target surface
        ANativeWindow_acquire(Window);
        ACameraOutputTarget_create(Window, &m_OutputTarget);
        ACaptureRequest_addTarget(m_CaptureRequest, m_OutputTarget);

        // Create the session
        ACameraDevice_createCaptureSession(m_Device, m_CaptureSessionOutputContainer, &m_CaptureSessionStateCallbacks, &m_CaptureSession);

        // Start capturing continuously
        ACameraCaptureSession_setRepeatingRequest(
            m_CaptureSession,
            &m_CaptureSessionCaptureCallbacks,
            1,
            &m_CaptureRequest,
            nullptr
        );
	}

	bool AndCamera::Open()
	{
		if (!Camera::Open())
			return false;

        m_Manager = ACameraManager_create();

        const std::string ID = PollID();
        camera_status_t Status = ACAMERA_OK;

        CB_CORE_INFO("Attempting to open camera (id: {0})", ID);

        Status = ACameraManager_openCamera(
            m_Manager,
            ID.c_str(),
            &m_DeviceStateCallbacks,
            &m_Device
        );
		
        if (Status != ACAMERA_OK)
        {
            CB_CORE_ERROR("Failed to open camera device (id: {0}, code: {1})", ID, Status);
            Camera::Close();
            return false;
        }
        else
        {
            CB_CORE_INFO("Camera device initialized (id: {0}, code: {1})", ID, Status);
        }

        Status = ACameraManager_getCameraCharacteristics(
            m_Manager,
            ID.c_str(),
            &m_Metadata
        );

        if (Status != ACAMERA_OK)
        {
            CB_CORE_ERROR("Failed to get camera meta data of ID (id: {0}, code: {1})", ID, Status);
        }

        return true;
	}

	bool AndCamera::Close()
	{
		if (!Camera::Close())
			return false;

        camera_status_t Status = ACAMERA_OK;

        if (m_Manager)
        {
            // Stop recording to SurfaceTexture and do some cleanup
            ACameraCaptureSession_stopRepeating(m_CaptureSession);
            ACameraCaptureSession_close(m_CaptureSession);
        	
            ACaptureSessionOutputContainer_free(m_CaptureSessionOutputContainer);
            ACaptureSessionOutput_free(m_SessionOutput);
            ACaptureSessionOutput_free(m_TextureOutput);
            ACameraOutputTarget_free(m_OutputTarget);

            ACameraMetadata_free(m_Metadata);

            Status = ACameraDevice_close(m_Device);

            if (Status != ACAMERA_OK)
            {
                CB_CORE_ERROR("Failed to close camera device (id: {0}, code: {1})", m_ID, Status);
            }
        	
            ACameraManager_delete(m_Manager);
            m_Manager = nullptr;

            // Capture request for SurfaceTexture
            // ANativeWindow_release(m_TextureWindow);
            ACaptureRequest_free(m_CaptureRequest);
        }
		
        return Status == ACAMERA_OK;
	}

    std::string AndCamera::PollID()
	{
        ACameraIdList* CameraIDs = nullptr;
        ACameraManager_getCameraIdList(m_Manager, &CameraIDs);

		CB_CORE_INFO("Found %d cameras", CameraIDs->numCameras);

        for (int i = 0; i < CameraIDs->numCameras; ++i)
        {
            const char* id = CameraIDs->cameraIds[i];

            ACameraMetadata* Metadata;
            ACameraManager_getCameraCharacteristics(m_Manager, id, &Metadata);

            ACameraMetadata_const_entry LensInfo = { 0 };
            ACameraMetadata_getConstEntry(Metadata, ACAMERA_LENS_FACING, &LensInfo);

            auto facing = static_cast<acamera_metadata_enum_android_lens_facing_t>
        	(
                LensInfo.data.u8[0]
            );

            // Found a back-facing camera?
            if (
                (m_Type == E_CAMERA_BACKFACE && facing == ACAMERA_LENS_FACING_BACK) ||
                (m_Type == E_CAMERA_FRONTFACE && facing == ACAMERA_LENS_FACING_FRONT)
            )
            {
                m_ID = id;
                ACameraManager_deleteCameraIdList(CameraIDs);
                return m_ID;
            }
        }

        CB_CORE_WARN("Could not find matching camera (type id: {0}, camera count : {1})", static_cast<int32_t>(m_Type), CameraIDs->numCameras);

        if (CameraIDs->numCameras > 0)
        {
            m_ID = CameraIDs->cameraIds[0];
            CB_CORE_WARN("Falling back to first camera match (id: {0})", CameraIDs->cameraIds[0]);
        }
		
        ACameraManager_deleteCameraIdList(CameraIDs);
        return m_ID;
	}

	void AndCamera::OnDisconnected(void* Context, ACameraDevice* Device)
	{
        CB_CORE_INFO("Camera is disconnected, {0}", ACameraDevice_getId(Device));
	}

	void AndCamera::OnError(void* Context, ACameraDevice* Device, int Error)
	{
        CB_CORE_ERROR("Error (code: {0}) on Camera (id: {1})", Error, ACameraDevice_getId(Device));
	}

	void AndCamera::OnReady(void* Context, ACameraCaptureSession* Session)
	{
        CB_CORE_INFO("Camera is ready");
	}
	
    void AndCamera::OnActive(void* Context, ACameraCaptureSession* Session)
    {
        CB_CORE_INFO("Camera is active");
    }

    void AndCamera::OnClosed(void* Context, ACameraCaptureSession* Session)
    {
        CB_CORE_INFO("Camera is closed");
    }

	bool AndCamera::RequestPermission()
	{
        GLFWwindow* Window = static_cast<GLFWwindow*>(
            Application::Get().GetRenderContext().GetWindow().GetWindow()
        );
		
        android_app* app = glfwGetAndroidApp(Window);
        if (!app) return false;

        JNIEnv* env;
        ANativeActivity* activity = app->activity;
        activity->vm->GetEnv((void**)&env, JNI_VERSION_1_6);

        activity->vm->AttachCurrentThread(&env, nullptr);

        jobject activityObj = env->NewGlobalRef(activity->clazz);
        jclass clz = env->GetObjectClass(activityObj);
        env->CallVoidMethod(activityObj,
        env->GetMethodID(clz, "RequestCamera", "()V"));
        env->DeleteGlobalRef(activityObj);

        activity->vm->DetachCurrentThread();
        return true;
	}

	void AndCamera::OnCameraPermission(jboolean Granted)
	{
        HasPermission = (Granted != JNI_FALSE);

        if (HasPermission)
        {
            InternalStart();
            return;
        }

        CB_CORE_WARN("Permissions to access the camera denied!");
	}
}

#endif