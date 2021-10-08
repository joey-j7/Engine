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

class StretchView : public SubView
{
public:
	StretchView(const String& FilePath, const String& Name = "Line View");
	
	void UpdateCameraUniforms();

private:
	void SetStartPosition(const Engine::DVector2& Delta, Engine::UILine& Line, Engine::UIButton& StartOval, Engine::UIButton& EndOval);
	void SetEndPosition(const Engine::DVector2& Delta, Engine::UILine& Line, Engine::UIButton& StartOval, Engine::UIButton& EndOval);
	void OnCameraImageData();

	Vector2 NormToAbs(Vector2 Normalized) const;
	Vector2 AbsToNorm(Vector2 Absolute) const;

	virtual void OnBack() override;
	void OnMirrorView();

	void RetrieveUserData();
	void SaveUserData();

	Engine::DrawPass* m_pPass = nullptr;

	Engine::StaticEntity* LineEntity1 = nullptr;
	Engine::UILine* Line1 = nullptr;
	Engine::UIButton* StartOval1 = nullptr;
	Engine::UIButton* EndOval1 = nullptr;

	Engine::StaticEntity* LineEntity2 = nullptr;
	Engine::UILine* Line2 = nullptr;
	Engine::UIButton* StartOval2 = nullptr;
	Engine::UIButton* EndOval2 = nullptr;

	Engine::json m_ConfigFile;
};