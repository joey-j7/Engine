#pragma once

#include "SubView.h"
#include "Rendering/Passes/DrawPass.h"

namespace Engine
{
	class UIButton;
	class UIImage;
	class UILine;
	class Entity;
}

using namespace Engine;

class LineView : public SubView
{
public:
	LineView(const String& m_FilePath, const String& Name = "Line View");
	
private:
	void SetStartPosition(const Engine::DVector2& Delta, Engine::UILine& Line, Engine::UIButton& StartOval, Engine::UIButton& EndOval);
	void SetEndPosition(const Engine::DVector2& Delta, Engine::UILine& Line, Engine::UIButton& StartOval, Engine::UIButton& EndOval);
	void OnCameraImageData();

	Vector2 NormToAbs(Vector2 Normalized) const;
	Vector2 AbsToNorm(Vector2 Absolute) const;

	void OnLineToggle();
	void OnCameraView();
	void OnStretchView();

	void RetrieveUserData();
	void SaveUserData();

	Engine::DrawPass* m_pPass = nullptr;

	Engine::Entity* LineEntity1 = nullptr;
	Engine::UILine* Line1 = nullptr;
	Engine::UIButton* StartOval1 = nullptr;
	Engine::UIButton* EndOval1 = nullptr;

	Engine::Entity* LineEntity2 = nullptr;
	Engine::UILine* Line2 = nullptr;
	Engine::UIButton* StartOval2 = nullptr;
	Engine::UIButton* EndOval2 = nullptr;

	Engine::UIButton* LineToggle = nullptr;
	bool m_LineToggled = false;

	Engine::json m_ConfigFile;
};