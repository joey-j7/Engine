#pragma once

#include "Engine/Objects/Worlds/World.h"
#include "Rendering/Passes/DrawPass.h"

#include "../PhotoListItem.h"

namespace Engine
{
	class Entity;
}

using namespace Engine;

class ListView : public World
{
public:
	ListView(const String& Name = "List View");

protected:
	virtual bool Play() override;

	void OnCameraClick();
	void OnPhotoView();

	void CreateHeader();
	void PopulateList();
	
	virtual void Draw(float fDeltaTime) override;
	
	Entity* m_Canvas = nullptr;
	UILayout* m_Layout = nullptr;

	std::vector<PhotoListItem*> m_ListItems;

	String m_ImagePath = "";
	String m_FolderPath = "";

	static float m_ScrollPosition;
};