#pragma once

#include "Engine/Files/Rendering/RenderFile.h"

namespace Engine
{
	class Engine_API TextureFile : public RenderFile
	{
	public:
		TextureFile(const String& filePath) : RenderFile(filePath)
		{
		}
	};
}