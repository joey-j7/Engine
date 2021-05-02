#include "pch.h"

#ifdef CB_PLATFORM_ANDROID
#include "Platform/FileLoader.h"

#include "Engine/Application.h"
#include "GLFW/glfw3native.h"
#include "Rendering/RenderContext.h"
#include "Platform/Window.h"

#include <android/asset_manager.h>

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

	char* FileLoader::Read(const std::string& filePath, uint32_t& fileLength, Type type, bool addNull)
	{
		// Read apk asset
		//if (type == E_CONTENT)
		//{
			AAsset* asset = AAssetManager_open(m_AssetManager, (m_WorkingDirectory[type] + filePath).c_str(), AASSET_MODE_STREAMING);

			if (!asset)
			{
				CB_CORE_ERROR("Could not open file at path \"{0}\"!", filePath);
				return {};
			}

			fileLength = AAsset_getLength(asset);

			char* fileContent = new char[addNull ? fileLength + 1 : fileLength];

			AAsset_read(asset, fileContent, fileLength);
			AAsset_close(asset);

			if (addNull)
				fileContent[fileLength] = '\0';

			CB_CORE_INFO("Loaded file at path \"{0}\"", filePath);

			return fileContent;
		//}

		return ReadStream(filePath, fileLength, type, addNull);
	}

	bool FileLoader::Exists(const std::string& filePath, Type type /*= E_CONTENT*/)
	{
		// Retrieve full absolute path
		std::string path = GetPath(filePath, type);
		
		// Read apk asset
		if (type == E_CONTENT)
		{
			AAsset* asset = AAssetManager_open(m_AssetManager, (m_WorkingDirectory[type] + filePath).c_str(), AASSET_MODE_STREAMING);
			bool isValid = asset;
			AAsset_close(asset);

			return isValid;
		}

		struct stat buffer;
		return (stat(path.c_str(), &buffer) == 0);
	}
}

#endif