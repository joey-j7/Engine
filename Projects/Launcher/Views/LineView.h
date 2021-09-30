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

class LineView : public Engine::World
{
public:
	LineView(const String& FilePath);

	void UpdateCameraUniforms();

private:
	void SetStartPosition(const Engine::DVector2& Delta, Engine::UILine& Line, Engine::UIButton& StartOval, Engine::UIButton& EndOval);
	void SetEndPosition(const Engine::DVector2& Delta, Engine::UILine& Line, Engine::UIButton& StartOval, Engine::UIButton& EndOval);
	void OnCameraImageData();

	void OnCameraView();
	void OnStretchView();

	Engine::DrawPass* m_pPass = nullptr;

	String m_Path = "";
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