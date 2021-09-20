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

class EditorView : public Engine::World
{
public:
	EditorView();

	void UpdateCameraUniforms();

protected:
	virtual void Draw(float fDeltaTime) override;

private:
	void SetStartPosition(const Engine::DVector2& Delta, Engine::UILine& Line, Engine::UIButton& EndOval);
	void SetEndPosition(const Engine::DVector2& Delta, Engine::UILine& Line, Engine::UIButton& EndOval);
	void OnCameraImageData();

	Engine::DrawPass* m_pPass = nullptr;

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