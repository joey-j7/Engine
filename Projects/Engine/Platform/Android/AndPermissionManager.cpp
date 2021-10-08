#include "pch.h"
#include "AndPermissionManager.h"

#include "Engine/Application.h"
#include "GLFW/glfw3native.h"

#include <memory.h>

#include <android_native_app_glue.h>
#include <jni.h>

// JNI to CPP callback for permission handling
extern "C"
{
	JNIEXPORT void JNICALL Java_com_appuil_Launcher_NativeLibrary_PermissionNotify(
		JNIEnv* Env, jclass Type, jobjectArray Permissions, jintArray GrantResults
	) {
		int32_t PermissionCount = Env->GetArrayLength(Permissions);
		jint* GrantArray = Env->GetIntArrayElements(GrantResults, 0);

		std::vector<String> PermissionStrings;
		std::vector<int32_t> GrantedResults;

		PermissionStrings.resize(PermissionCount);
		GrantedResults.resize(PermissionCount);

		for (int i = 0; i < PermissionCount; i++) {
			PermissionStrings[i] = Env->GetStringUTFChars((jstring)(Env->GetObjectArrayElement(Permissions, i)), NULL);
			GrantedResults[i] = GrantArray[i];
		}

		std::thread PermissionHandler(
			&Engine::PermissionManager::OnPermission,
			&Engine::PermissionManager::Get(),
			PermissionStrings,
			GrantedResults
		);

		PermissionHandler.detach();
	}
};

namespace Engine
{
	AndPermissionManager::AndPermissionManager()
	{
	}

	PermissionManager::Type AndPermissionManager::StringToType(const String& String)
	{
		if (String == "android.permission.CAMERA")
			return E_CAMERA;

		else if (String == "android.permission.READ_EXTERNAL_STORAGE")
			return E_READ_EXTERNAL_STORAGE;

		else if (String == "android.permission.WRITE_EXTERNAL_STORAGE")
			return E_WRITE_EXTERNAL_STORAGE;
		
		return E_UNKNOWN;
	}

	bool AndPermissionManager::RequestPermission(Type Permission)
	{
		return RequestPermissions({ Permission });
	}

	bool AndPermissionManager::RequestPermissions(const std::vector<Type>& Permissions)
	{
		if (m_RequestingPermissions)
			return false;

		// Check if permissions aren't already granted
		bool Granted = true;

		for (auto Permission : Permissions)
		{
			if (!HasPermission(Permission))
			{
				Granted = false;
				break;
			}
		}

		// Already granted, nothing to do here
		if (Granted)
			return true;

		GLFWwindow* Window = static_cast<GLFWwindow*>(
			Application::Get().GetRenderContext().GetWindow().GetWindow()
		);

		android_app* App = glfwGetAndroidApp(Window);
		if (!App) return false;

		JNIEnv* Env;
		ANativeActivity* Activity = App->activity;
		Activity->vm->GetEnv((void**)&Env, JNI_VERSION_1_6);

		if (!Env)
		{
			Activity->vm->AttachCurrentThread(&Env, nullptr);
		}

		if (!Env)
			return false;

		jobject ActivityObj = Env->NewGlobalRef(Activity->clazz);
		jclass Clz = Env->GetObjectClass(ActivityObj);
		auto Method = Env->GetMethodID(Clz, "RequestPermissions", "([Ljava/lang/String;)V");

		if (!Method)
			return false;

		m_RequestingPermissions = true;

		// Create the string array
		jclass StringClass = Env->FindClass("java/lang/String");
		jobjectArray PermissionStrings = Env->NewObjectArray(Permissions.size(), StringClass, NULL);

		for (int32_t i = 0; i < Permissions.size(); ++i)
		{
			String Permission = "";

			switch (Permissions[i])
			{
			case E_CAMERA:
				Permission = "android.permission.CAMERA";
				break;
			case E_READ_EXTERNAL_STORAGE:
				Permission = "android.permission.READ_EXTERNAL_STORAGE";
				break;
			case E_WRITE_EXTERNAL_STORAGE:
				Permission = "android.permission.WRITE_EXTERNAL_STORAGE";
				break;
			default:
				CB_CORE_WARN("Unsupported permission type {0} detected", Permissions[i]);
				continue;
			}

			jstring PermissionString = Env->NewStringUTF(Permission.c_str());
			Env->SetObjectArrayElement(PermissionStrings, i, PermissionString);
		}

		// Call the permission method
		Env->CallVoidMethod(ActivityObj, Method, PermissionStrings);

		Env->DeleteLocalRef(PermissionStrings);
		Env->DeleteGlobalRef(ActivityObj);

		Activity->vm->DetachCurrentThread();

		return true;
	}
}
