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

using namespace Engine;

class MirrorView : public SubView
{
public:
	MirrorView(const String& FilePath, const String& Name = "Line View");
	
	void UpdateCameraUniforms();

private:
	void OnCameraImageData();

	Vector2 NormToAbs(Vector2 Normalized) const;
	Vector2 AbsToNorm(Vector2 Absolute) const;

	virtual void OnBack() override;
	void OnListView();
	void OnMirrorToggle();

	void RetrieveUserData();
	void SaveUserData();

	void ToggleButtons();

	Engine::DrawPass* m_pPass = nullptr;

	Engine::StaticEntity* LineEntity1 = nullptr;
	Engine::UILine* Line1 = nullptr;
	Engine::UIButton* StartOval1 = nullptr;
	Engine::UIButton* EndOval1 = nullptr;

	Engine::StaticEntity* LineEntity2 = nullptr;
	Engine::UILine* Line2 = nullptr;
	Engine::UIButton* StartOval2 = nullptr;
	Engine::UIButton* EndOval2 = nullptr;
	
	Engine::StaticEntity* TextEntity = nullptr;
	Engine::UIButton* NextButton;

	Engine::UIButton* MirrorToggle = nullptr;
	uint32_t MirrorCount = 0;
	
	Engine::json m_ConfigFile;
};