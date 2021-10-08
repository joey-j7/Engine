#pragma once

#include "Engine/Objects/Worlds/World.h"
#include "Rendering/Passes/DrawPass.h"

#include "Engine/Objects/Worlds/Entities/UI/UIButton.h"

namespace Engine
{
	class StaticEntity;
}

using namespace Engine;

class ListView : public World
{
public:
	ListView(const String& Name = "List View");

protected:
	void OnCameraClick();
	void OnPhotoView();

	void PopulateList();
	void OnPermission(const std::vector<String>& Permissions, const std::vector<int32_t>& Granted);

	virtual void Draw(float fDeltaTime) override;
	
	StaticEntity* m_Canvas = nullptr;
	std::vector<UIButton*> m_ListItems;

	String m_ImagePath = "";
	String m_FolderPath = "";
};