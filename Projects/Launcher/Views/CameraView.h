#pragma once

#include "SubView.h"
#include "Rendering/Passes/DrawPass.h"

namespace Engine
{
	class UIButton;
	class UIImage;
	class UILine;
	class StaticEntity;
}

class CameraView : public SubView
{
public:
	CameraView(const String& Name = "Camera View");
	~CameraView();

private:
	void OnCameraImageData();
	void OnLineView(const String& Path);
	
	void OnPhotoTaken(const String& FilePath);

	uint32_t m_ImageDataEventID = 0;

	Engine::UIImage* CameraImage = nullptr;
};