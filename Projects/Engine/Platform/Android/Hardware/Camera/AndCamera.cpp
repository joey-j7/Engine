#include "pch.h"
#include "AndCamera.h"

#ifdef CB_PLATFORM_ANDROID

#include <android_native_app_glue.h>
#include <memory.h>

#include "GLFW/glfw3native.h"
#include "Engine/Application.h"

#include "Engine/Objects/Worlds/Entities/Components/UI/Renderables/UIImage.h"

#include <include/core/SkYUVAPixmaps.h>
#include <include/core/SkYUVAInfo.h>
#include <include/core/SkImageEncoder.h>

#include <jni.h>

extern "C"
{
    JNIEXPORT void JNICALL Java_com_appuil_Launcher_NativeLibrary_notifyCameraPermission(
        JNIEnv* env, jclass type, jboolean permission) {
        std::thread permissionHandler(
            &Engine::AndCamera::OnPermission,
            (Engine::AndCamera*)&Engine::Application::Get().GetHardwareContext().GetCamera(),
            permission
        );
        permissionHandler.detach();
    }
	
    /*JNIEXPORT void JNICALL Java_com_appuil_Launcher_NativeLibrary_drawCameraFrame(
        JNIEnv* env, jclass type, jbyteArray data, jint width, jint height, jint rotation) {
    	
        jbyte* bufferPtr = env->GetByteArrayElements(data, 0);
		jsize arrayLength = env->GetArrayLength(data);

        Engine::AndCamera* Camera = (Engine::AndCamera*)&Engine::Application::Get().GetHardwareContext().GetCamera();
        if (Camera) Camera->OnFrame(bufferPtr, arrayLength, width, height, rotation);
    	
        env->ReleaseByteArrayElements(data, bufferPtr, 0);
    }*/
};

namespace Engine
{
    AndCamera::CaptureState AndCamera::m_CaptureState = E_CLOSED;
    std::mutex AndCamera::m_Mutex;

    AImageReader* AndCamera::m_Reader = nullptr;
    AImageReader_ImageListener AndCamera::m_PreviewImageCallbacks;
    AImageReader_ImageListener AndCamera::m_PhotoImageCallbacks;

    bool AndCamera::m_IsCapturing = false;
    bool AndCamera::m_IsTakingPhoto = false;

	AndCamera::AndCamera()
	{
        m_DeviceStateCallbacks = {
            .context = this,
            .onDisconnected = AndCamera::OnDisconnected,
            .onError = AndCamera::OnError
        };

        m_CaptureSessionStateCallbacks = {
            .context = this,
            .onActive = AndCamera::OnActive,
            .onReady = AndCamera::OnReady,
            .onClosed = AndCamera::OnClosed
        };

        m_CaptureSessionCaptureCallbacks = {
            .context = this,
            .onCaptureBufferLost = AndCamera::OnCaptureBufferLost,
            .onCaptureCompleted = AndCamera::OnCaptureCompleted,
            .onCaptureFailed = AndCamera::OnCaptureFailed,
            .onCaptureProgressed = AndCamera::OnCaptureProgressed,
            .onCaptureSequenceAborted = AndCamera::OnCaptureSequenceAborted,
            .onCaptureSequenceCompleted = AndCamera::OnCaptureSequenceCompleted,
            .onCaptureStarted = AndCamera::OnCaptureStarted
        };
        
        m_PreviewImageCallbacks = {
            .context = &m_PreviewImage,
            .onImageAvailable = AndCamera::OnPreviewRetrieved,
        };

        m_PhotoImageCallbacks = {
            .context = this,
            .onImageAvailable = AndCamera::OnPhotoRetrieved,
        };
	}

	AndCamera::~AndCamera()
	{
        Close();
	}

    bool AndCamera::Start(CameraType Type)
    {
        if (m_bStarting)
            return true;

        if (m_bStarted)
        {
            if (Type != m_Type)
                Stop();
            else
                return false;
        }

        std::lock_guard<std::mutex> Guard(m_Mutex);
        m_bStarting = true;

		// Permission thread will call once it has received the permission status
        if (!RequestPermission())
        {
            m_bStarting = false;
            return false;
        }

        m_Type = Type;
        
        return true;
    }

    bool AndCamera::Stop()
    {
        if (!Camera::Stop())
            return false;

        m_DelayedStart = false;

        return true;
    }

    bool AndCamera::IsReady()
    {
        return m_CaptureState == E_READY || m_CaptureState == E_CLOSED;
    }

	void AndCamera::DelayedStart()
	{	
        if (!Camera::Start(m_Type) || m_DelayedStart)
            return;

        StartPreview();

        std::lock_guard<std::mutex> Guard(m_Mutex);
        m_DelayedStart = true;

        OnStartCallback();

        m_bStarting = false;
	}

    void AndCamera::TakePhoto()
    {
        if (!m_bStarted || !m_HasPermission)
        {
            CB_CORE_WARN("Cannot take photo if camera has not been started yet");
            return;
        }

        if (!m_IsCapturing)
        {
            CB_CORE_WARN("Cannot take photo if capture session has not been started yet");
            return;
        }

        m_IsTakingPhoto = true;

        // Capture photo
        AImageReader_setImageListener(m_Reader, &m_PhotoImageCallbacks);
        CB_CORE_INFO("Taking photo capture");
    }

    void AndCamera::StartPreview()
    {
        std::lock_guard<std::mutex> Guard(m_Mutex);

        if (m_IsCapturing)
        {
            CB_CORE_WARN("Could not start preview because there's another active capture");
            return;
        }

        // Wait until closed
        while (m_CaptureState != E_CLOSED) {}

        m_IsCapturing = true;

        m_CaptureState = E_INITIALIZING;
        Window& Window = Application::Get().GetRenderContext().GetWindow();

        // Create Image Reader
        media_status_t Status = AImageReader_new(
            m_Orientation == 90.f || m_Orientation == 270.f ? Window.GetHeight() : Window.GetWidth(),
            m_Orientation == 90.f || m_Orientation == 270.f ? Window.GetWidth() : Window.GetHeight(),
            AIMAGE_FORMAT_JPEG,
            4,
            &m_Reader
        );

        if (Status != AMEDIA_OK)
        {
            CB_CORE_ERROR("Could not create ImageReader for camera preview");
            
            m_CaptureState = E_CLOSED;
            m_IsCapturing = false;

            return;
        }
        
        Status = AImageReader_getWindow(m_Reader, &m_PreviewWindow);

        if (Status != AMEDIA_OK)
        {
            CB_CORE_INFO("Failed to acquire and attach ImageReader window for camera preview");
            
            m_CaptureState = E_CLOSED;
            m_IsCapturing = false;

            return;
        }

        AImageReader_setImageListener(m_Reader, &m_PreviewImageCallbacks);
        
        // Prepare request for texture target
        const camera_status_t CamStatus = ACameraDevice_createCaptureRequest(m_Device, TEMPLATE_PREVIEW, &m_CaptureRequest);

        if (CamStatus != ACAMERA_OK)
        {
            CB_CORE_ERROR("Failed to create preview capture request (id: {0}, code: {1})", m_ID, CamStatus);
        }

        // Prepare outputs for session
        ACaptureSessionOutput_create(m_PreviewWindow, &m_TextureOutput);
        ACaptureSessionOutputContainer_create(&m_CaptureSessionOutputContainer);
        ACaptureSessionOutputContainer_add(m_CaptureSessionOutputContainer, m_TextureOutput);

        // Prepare target surface
        ANativeWindow_acquire(m_PreviewWindow);
        ACameraOutputTarget_create(m_PreviewWindow, &m_OutputTarget);
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

        CB_CORE_INFO("Starting photo preview capture");
    }

    void AndCamera::StopCapture()
    {
        if (!m_IsCapturing)
        {
            CB_CORE_WARN("Tried to stop capture while it was already stopped");
            return;
        }

        m_IsTakingPhoto = false;

        std::lock_guard<std::mutex> Guard(m_Mutex);

        if (m_CaptureSession)
        {
            ACameraCaptureSession_stopRepeating(m_CaptureSession);
            ACameraCaptureSession_abortCaptures(m_CaptureSession);
            ACameraCaptureSession_close(m_CaptureSession);

            m_CaptureSession = nullptr;
        }

        if (m_CaptureSessionOutputContainer)
        {
            ACaptureSessionOutputContainer_free(m_CaptureSessionOutputContainer);
            m_CaptureSessionOutputContainer = nullptr;
        }

        if (m_SessionOutput)
        {
            ACaptureSessionOutput_free(m_SessionOutput);
            m_SessionOutput = nullptr;
        }

        if (m_TextureOutput)
        {
            ACaptureSessionOutput_free(m_TextureOutput);
            m_TextureOutput = nullptr;
        }

        if (m_OutputTarget)
        {
            ACameraOutputTarget_free(m_OutputTarget);
            m_OutputTarget = nullptr;
        }

        if (m_CaptureRequest)
        {
            ACaptureRequest_free(m_CaptureRequest);
            m_CaptureRequest = nullptr;
        }

        if (m_Reader)
        {
            AImageReader_delete(m_Reader);
            m_Reader = nullptr;
        }
		
        if (m_PreviewWindow)
        {
            ANativeWindow_release(m_PreviewWindow);
            m_PreviewWindow = nullptr;
        }

        if (m_PreviewImage)
        {
            m_PreviewImage->SetImageData(nullptr, 0);

            // Managed by the object that sets it, because the image might get destroyed
            // while the camera stays alive during view switches!
            // m_PreviewImage = nullptr;
        }

        m_CaptureState = E_CLOSED;
        m_IsCapturing = false;
    }

	void AndCamera::OnPhotoProcessed()
	{
        // Start capturing continuously
        ACameraCaptureSession_setRepeatingRequest(
            m_CaptureSession,
            &m_CaptureSessionCaptureCallbacks,
            1,
            &m_CaptureRequest,
            nullptr
        );
		
        OnPhotoTakenCallback(m_LastPhotoPath);
	}
    
	bool AndCamera::Open()
	{
		if (!Camera::Open())
			return false;

        m_Manager = ACameraManager_create();

        const String ID = PollID();
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

        return true;
	}

	bool AndCamera::Close()
	{
		if (!Camera::Close())
			return false;

        camera_status_t Status = ACAMERA_OK;

        StopCapture();

        if (m_Manager)
        {
            // Lock
            std::lock_guard<std::mutex> Guard(m_Mutex);

            ACameraMetadata_free(m_Metadata);

            Status = ACameraDevice_close(m_Device);

            if (Status != ACAMERA_OK)
            {
                CB_CORE_ERROR("Failed to close camera device (id: {0}, code: {1})", m_ID, Status);
            }
        	
            ACameraManager_delete(m_Manager);
            m_Manager = nullptr;
        }

        // Cancel any pending requests
        m_DelayedStart = false;
        m_bStarting = false;
        m_bStarted = false;

        return Status == ACAMERA_OK;
	}

    String AndCamera::PollID()
	{
        ACameraIdList* CameraIDs = nullptr;
        ACameraManager_getCameraIdList(m_Manager, &CameraIDs);

		CB_CORE_INFO("Found {0} cameras", CameraIDs->numCameras);

        for (int i = 0; i < CameraIDs->numCameras; ++i)
        {
            const char* id = CameraIDs->cameraIds[i];
            
            const auto Status = ACameraManager_getCameraCharacteristics(
                m_Manager,
                id,
                &m_Metadata
            );

            if (Status != ACAMERA_OK)
            {
                CB_CORE_ERROR("Failed to get camera meta data of ID (id: {0}, code: {1})", id, Status);
            }

            ACameraMetadata_const_entry LensInfo = { 0 };
            ACameraMetadata_getConstEntry(m_Metadata, ACAMERA_LENS_FACING, &LensInfo);

            auto facing = static_cast<acamera_metadata_enum_android_lens_facing_t>
        	(
                LensInfo.data.u8[0]
            );

            // Found a back-facing camera?
            if (
                (m_Type == E_CAMERA_BACKFACE && facing == ACAMERA_LENS_FACING_BACK) ||
                (m_Type == E_CAMERA_FRONTFACE && facing == ACAMERA_LENS_FACING_FRONT) ||
                (m_Type == E_CAMERA_EXTERNAL && facing == ACAMERA_LENS_FACING_EXTERNAL)
            )
            {
                m_ID = id;
            	
                ACameraMetadata_const_entry entry = { 0 };
                ACameraMetadata_getConstEntry(m_Metadata, ACAMERA_SENSOR_ORIENTATION, &entry);

                m_Orientation = entry.data.i32[0];

                ACameraManager_deleteCameraIdList(CameraIDs);

                return m_ID;
            }
        }

        CB_CORE_WARN("Could not find matching camera (type id: {0}, camera count : {1})", static_cast<int32_t>(m_Type), CameraIDs->numCameras);

        if (CameraIDs->numCameras > 0)
        {
            m_ID = CameraIDs->cameraIds[0];
            CB_CORE_WARN("Falling back to first camera match (id: {0})", CameraIDs->cameraIds[0]);

            const auto Status = ACameraManager_getCameraCharacteristics(
                m_Manager,
                m_ID.c_str(),
                &m_Metadata
            );

            if (Status != ACAMERA_OK)
            {
                CB_CORE_ERROR("Failed to get camera meta data of ID (id: {0}, code: {1})", m_ID, Status);
            }

            ACameraMetadata_const_entry entry = { 0 };
            ACameraMetadata_getConstEntry(m_Metadata, ACAMERA_SENSOR_ORIENTATION, &entry);

            m_Orientation = entry.data.i32[0];
        }
		
        ACameraManager_deleteCameraIdList(CameraIDs);
        return m_ID;
	}

	void AndCamera::OnDisconnected(void* Context, ACameraDevice* Device)
	{
        CB_CORE_TRACE("Camera is disconnected, {0}", ACameraDevice_getId(Device));
	}

	void AndCamera::OnError(void* Context, ACameraDevice* Device, int Error)
	{
        CB_CORE_ERROR("Error (code: {0}) on Camera (id: {1})", Error, ACameraDevice_getId(Device));
	}

	void AndCamera::OnReady(void* Context, ACameraCaptureSession* Session)
	{
        CB_CORE_TRACE("Camera capture session is ready");
        AndCamera::m_CaptureState = E_READY;
	}
	
    void AndCamera::OnActive(void* Context, ACameraCaptureSession* Session)
    {
        CB_CORE_TRACE("Camera capture session is active");
        AndCamera::m_CaptureState = E_ACTIVE;
    }

    void AndCamera::OnClosed(void* Context, ACameraCaptureSession* Session)
    {
        CB_CORE_TRACE("Camera capture session is closed");
        AndCamera::m_CaptureState = E_CLOSED;
    }
    
    void AndCamera::OnCaptureBufferLost(void* context, ACameraCaptureSession* session, ACaptureRequest* request, ACameraWindowType* window, int64_t frameNumber)
	{
        CB_CORE_TRACE("Camera capture buffer is lost");
	}
	
    void AndCamera::OnCaptureCompleted(void* context, ACameraCaptureSession* session, ACaptureRequest* request, const ACameraMetadata* result)
	{
        // CB_CORE_TRACE("Camera capture has completed");
	}
	
    void AndCamera::OnCaptureFailed(void* context, ACameraCaptureSession* session, ACaptureRequest* request, ACameraCaptureFailure* failure)
	{
        CB_CORE_ERROR("Camera capture has failed, reason: {0}", failure->reason);
	}
	
    void AndCamera::OnCaptureProgressed(void* context, ACameraCaptureSession* session, ACaptureRequest* request, const ACameraMetadata* result)
	{
        // CB_CORE_TRACE("Camera capture has progressed");
	}
	
    void AndCamera::OnCaptureSequenceAborted(void* context, ACameraCaptureSession* session, int sequenceId)
	{
        CB_CORE_TRACE("Camera capture sequence is aborted");
	}
	
    void AndCamera::OnCaptureSequenceCompleted(void* context, ACameraCaptureSession* session, int sequenceId, int64_t frameNumber)
	{
        // CB_CORE_TRACE("Camera capture sequence has completed");
	}
	
    void AndCamera::OnCaptureStarted(void* context, ACameraCaptureSession* session, const ACaptureRequest* request, int64_t timestamp)
	{
        // CB_CORE_TRACE("Camera capture sequence has started");
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

        if (!env)
        {
            activity->vm->AttachCurrentThread(&env, nullptr);
        }

        if (!env)
            return false;

        jobject activityObj = env->NewGlobalRef(activity->clazz);
        jclass clz = env->GetObjectClass(activityObj);
        auto method = env->GetMethodID(clz, "RequestCamera", "()V");

        if (!method)
            return false;
		
        env->CallVoidMethod(activityObj, method);
        env->DeleteGlobalRef(activityObj);
        
        activity->vm->DetachCurrentThread();
        
        return true;
	}

    void AndCamera::ResetReader()
    {
        if (!m_Reader)
            return;

        // Reset reader to preview callback
        AImageReader_setImageListener(m_Reader, &m_PreviewImageCallbacks);
    }

	void AndCamera::OnPermission(jboolean Granted)
	{
        m_HasPermission = (Granted != JNI_FALSE);

        if (m_HasPermission)
        {
            DelayedStart();
            return;
        }

        std::lock_guard<std::mutex> Guard(m_Mutex);

        CB_CORE_WARN("Permissions to access the camera denied!");
        m_bStarting = false;
	}

	void AndCamera::OnPreviewRetrieved(void* Context, AImageReader* Reader)
	{
        UIImage** CameraImage = static_cast<UIImage**>(Context);

		if (!CameraImage || !*CameraImage)
            return;

        AImage* Image = nullptr;
        const media_status_t Status = AImageReader_acquireNextImage(Reader, &Image);
        
        if (Status != AMEDIA_OK)
        {
            CB_CORE_ERROR("Could not retrieve image");
            return;
        }

        uint8_t* Data = nullptr;
        int Length = 0;
        AImage_getPlaneData(Image, 0, &Data, &Length);

        int32_t Width, Height;
        AImage_getWidth(Image, &Width);
        AImage_getHeight(Image, &Height);

        UIImage& CamImage = **CameraImage;
        CamImage.SetImageData((char*)Data, Length);
        	
        AImage_delete(Image);
	}
	
    void AndCamera::OnPhotoRetrieved(void* Context, AImageReader* Reader)
    {
        if (!AndCamera::IsCapturing() || !AndCamera::m_IsTakingPhoto)
            return;

        std::lock_guard<std::mutex> Guard(AndCamera::m_Mutex);

        // Reset listener so this is called once
        AndCamera::ResetReader();

        AImage* Image = nullptr;
        const media_status_t Status = AImageReader_acquireNextImage(Reader, &Image);

        if (Status != AMEDIA_OK)
        {
            CB_CORE_ERROR("Could not retrieve photo");

            AndCamera* Camera = static_cast<AndCamera*>(Context);
            Application::Get().ThreadedCallback.Bind(Camera, &AndCamera::OnPhotoProcessed);

            AndCamera::m_IsTakingPhoto = false;

            return;
        }

        uint8_t* Data = nullptr;
        int Length = 0;
        AImage_getPlaneData(Image, 0, &Data, &Length);

        sk_sp<SkData> RotatedData;

        String Path = "/storage/emulated/0/DCIM/Appuil/" + Application::Get().GetName() + "/";
        String Filename = Time::GetFormattedString("%d_%m_%Y_%H_%M_%S") + ".jpg";

        AndCamera* Camera = static_cast<AndCamera*>(Context);

        // Rotate output accordingly
        int32_t Orientation = Camera->GetOrientation();

        if (Orientation == 90 || Orientation == 270)
        {
            sk_sp<SkImage> Image = SkImage::MakeFromEncoded(
                SkData::MakeWithoutCopy(Data, Length)
            );

            // Create temp canvas and surface
            sk_sp<SkSurface> Surface = SkSurface::MakeRasterN32Premul(
                Image->height(),
                Image->width()
            );

            SkCanvas* Canvas = Surface->getCanvas();

            // SkCanvas Canvas = SkCanvas(Image->height(), Image->width());
            // sk_sp<SkSurface> Surface = Canvas.makeSurface(Image->imageInfo());

            Canvas->translate(
                Orientation == 270 ? 0 : Image->height(),
                Orientation == 270 ? Image->width() : 0
            );
            
            Canvas->rotate(Orientation);
            Canvas->drawImage(Image, 0, 0);

            sk_sp<SkImage> Rotated(Surface->makeImageSnapshot());

            RotatedData = Rotated->encodeToData(SkEncodedImageFormat::kJPEG, 80);

            Data = (uint8_t*)RotatedData->data();
            Length = RotatedData->size();
        }

        if (!FileLoader::Write(Path, Filename, (char*)Data, Length, true, FileLoader::E_ROOT))
        {
            CB_CORE_ERROR("Photo could not be written to disk");
        }
        else
        {
            CB_CORE_INFO("Photo has successfully been taken");
            Camera->m_LastPhotoPath = Path + Filename;
        }

        AImage_delete(Image);

        // If cancelled
        if (!AndCamera::m_IsTakingPhoto)
        {
            FileLoader::Delete(Path, Filename, FileLoader::E_ROOT);
        }

        AndCamera::m_IsTakingPhoto = false;
        Application::Get().ThreadedCallback.Bind(Camera, &AndCamera::OnPhotoProcessed);
    }
}

#endif