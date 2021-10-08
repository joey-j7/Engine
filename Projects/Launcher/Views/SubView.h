#pragma once

#include "Engine/Objects/Worlds/World.h"

namespace Engine
{
	class UIButton;
	class UIImage;
}

class PhotoEntity;

class SubView : public Engine::World
{
public:
	SubView(const String& Path, const String& Name = "Sub View");
	~SubView();

protected:
	void OnKeyPressed(int32_t Key, int32_t Repeat);

	String m_Path = "";

	PhotoEntity* m_Photo = nullptr;
	Engine::UIImage* m_CameraImage = nullptr;

	Engine::UIButton* m_BackButton = nullptr;
	uint32_t m_KeyHandler = UINT_MAX;

	virtual void OnBack();
};