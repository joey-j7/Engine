#pragma once

#include "Engine/Objects/Worlds/Entities/StaticEntity.h"

namespace Engine
{
	class UIImage;
}

class PhotoEntity : public Engine::StaticEntity
{
public:
	PhotoEntity(const String& FilePath, const String& Name = "Photo Entity");

	Engine::UIImage* GetPhoto() const { return Photo; }

protected:
	Engine::UIImage* Photo = nullptr;
};