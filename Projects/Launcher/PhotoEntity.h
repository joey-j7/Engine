#pragma once

#include "Engine/Objects/Worlds/Entities/StaticEntity.h"

namespace Engine
{
	class UIImage;
}

using namespace Engine;

class PhotoEntity : public Engine::StaticEntity
{
public:
	PhotoEntity(const String& FilePath, const String& Name = "Photo Entity");

	UIImage* GetPhoto() const { return Photo; }

protected:
	UIImage* Photo = nullptr;
};