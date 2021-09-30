#pragma once

#include "Engine/Objects/Worlds/World.h"
#include "Rendering/Passes/DrawPass.h"

namespace Engine
{
	class UIButton;
	class UIImage;
	class UILine;
	class StaticEntity;
}

class CameraView : public Engine::World
{
public:
	CameraView();
	~CameraView();

private:
	void OnCameraImageData();
	void OnLineView(const String& Path);
	void OnListView();

	void SetStartPosition(const Engine::DVector2& Delta, Engine::UILine& Line, Engine::UIButton& EndOval);
	void SetEndPosition(const Engine::DVector2& Delta, Engine::UILine& Line, Engine::UIButton& EndOval);

	void UpdateCameraUniforms();

	void OnPhotoTaken(const String& FilePath);

	uint32_t m_ImageDataEventID = 0;

	Engine::UIImage* CameraImage = nullptr;

	Engine::StaticEntity* LineEntity1 = nullptr;
	Engine::UILine* Line1 = nullptr;
	Engine::UIButton* StartOval1 = nullptr;
	Engine::UIButton* EndOval1 = nullptr;

	Engine::StaticEntity* LineEntity2 = nullptr;
	Engine::UILine* Line2 = nullptr;
	Engine::UIButton* StartOval2 = nullptr;
	Engine::UIButton* EndOval2 = nullptr;
};