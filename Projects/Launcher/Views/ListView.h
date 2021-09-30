#pragma once

#include "Engine/Objects/Worlds/World.h"
#include "Rendering/Passes/DrawPass.h"

class ListView : public Engine::World
{
public:
	ListView();

protected:
	void OnCameraClick();
	void OnPhotoView();

	virtual void Draw(float fDeltaTime) override;

	String m_ImagePath = "";
	String m_FolderPath = "";
};