#pragma once

#include "Engine/Objects/Worlds/World.h"
#include "Rendering/Passes/DrawPass.h"

namespace Engine
{
	class UIButton;
	class UIImage;
	class UILine;
	class Entity;
}

class CameraView : public Engine::World
{
public:
	CameraView(const String& Name = "Camera View");
	~CameraView();

private:
	void OnCameraImageData();
	void OnLineView(const String& Path);
	
	void OnListView();
	void OnTrashView();
	
	void OnPhotoTaken(const String& m_FilePath);

	uint32_t m_ImageDataEventID = 0;

	Engine::UIImage* CameraImage = nullptr;
};