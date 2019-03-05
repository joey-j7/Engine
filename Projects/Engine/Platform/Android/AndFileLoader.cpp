#include "pch.h"

#ifdef CB_PLATFORM_ANDROID
#include "Platform/FileLoader.h"

#include "Engine/Application.h"
#include "GLFW/glfw3native.h"
#include "Rendering/RenderContext.h"
#include "Platform/Window.h"

#include "../../../arch-arm/usr/include/android/asset_manager.h"

namespace Engine
{
	static AAssetManager* m_AssetManager = nullptr;

	void FileLoader::Init()
	{
		// Get activity
		struct android_app* app = glfwGetAndroidApp(
			(GLFWwindow*)Application::Get().GetRenderContext().GetWindow().GetWindow()
		);
		auto* activity = app->activity;

		// Set asset manager
		m_AssetManager = activity->assetManager;

		// Set working directory
		m_WorkingDirectory[E_INTERNAL] = std::string(
			activity->internalDataPath
		) + "/";

		m_WorkingDirectory[E_EXTERNAL] = std::string(
			activity->externalDataPath
		) + "/";

		m_WorkingDirectory[E_CONTENT] = "Content/";
	}

	std::vector<char> FileLoader::Read(std::string filePath, Type type)
	{
		// Read apk asset
		if (type == E_CONTENT)
		{
			AAsset* asset = AAssetManager_open(m_AssetManager, (m_WorkingDirectory[type] + filePath).c_str(), AASSET_MODE_STREAMING);

			if (!asset)
			{
				CB_CORE_ERROR_T(5.0f, "Could not open file at path {0}!", filePath);
				return {};
			}

			size_t fileLength = AAsset_getLength(asset);

			std::vector<char> fileContent;
			fileContent.resize(fileLength);

			AAsset_read(asset, fileContent.data(), fileLength);
			AAsset_close(asset);

			CB_CORE_INFO_T(5.0f, "Loaded file at path {0}", filePath);

			return fileContent;
		}

		return ReadStream(filePath, type);
	}
}

#endif