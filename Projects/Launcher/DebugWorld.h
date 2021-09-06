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

class DebugWorld : public Engine::World
{
public:
	DebugWorld();

	void UpdateCameraUniforms();

protected:
	virtual void Draw(float fDeltaTime) override;

private:
	void OnCameraImageData();

	Engine::DrawPass* m_pPass = nullptr;

	Engine::UIImage* CameraImage = nullptr;

	Engine::StaticEntity* LineEntity = nullptr;
	Engine::UILine* Line = nullptr;
	Engine::StaticEntity* LineEntity2 = nullptr;
	Engine::UILine* Line2 = nullptr;

	Engine::UIButton* Oval1 = nullptr;
	Engine::UIButton* Oval2 = nullptr;
	Engine::UIButton* Oval3 = nullptr;
	Engine::UIButton* Oval4 = nullptr;
};