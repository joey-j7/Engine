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

class PhotoView : public Engine::World
{
public:
	PhotoView(const String& FilePath);

private:
	void OnLineView();
	void OnListView();

	Engine::UIImage* CameraImage = nullptr;
	String m_Path = "";
};