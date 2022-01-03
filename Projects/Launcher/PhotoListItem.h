#pragma once

using namespace Engine;

#include "Engine/Objects/Entities/UI/UIButton.h"

class PhotoListItem : public UIButton
{
public:
	PhotoListItem(
		World& World,
		const String& Path,
		const ButtonStyle& DefaultStyle = ButtonStyle(),
		const ButtonStyle& HoverStyle = ButtonStyle(),
		const ButtonStyle& PressStyle = ButtonStyle()
	);

protected:
	void LoadImage();
	sk_sp<SkImage> MakeThumbnail();

	String m_FilePath = "";
	UIImage* m_Photo = nullptr;

	sk_sp<SkImage> m_Thumbnail;

	Vector2 m_Dimensions;
};

