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

class PhotoEntity;

class PhotoView : public SubView
{
public:
	PhotoView(const String& FilePath, const String& Name = "Photo View");
	~PhotoView();

private:
	void RetrieveUserData();
	void UpdateCameraUniforms();

	Engine::Vector2 NormToAbs(Engine::Vector2 Normalized) const;

	void OnLineView();
	void OnDelete();

	void ToggleButtons();

	Engine::json m_ConfigFile;
	uint32_t MirrorCount = 0;
	uint32_t LineCount = 0;

	Engine::UIButton* m_EditButton = nullptr;
	Engine::UIButton* m_DeleteButton = nullptr;
};