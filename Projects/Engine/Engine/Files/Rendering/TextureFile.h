#pragma once

#include "Engine/Files/Rendering/RenderFile.h"

namespace Engine
{
	class Engine_API TextureFile : public RenderFile
	{
	public:
		TextureFile(const std::string& filePath) : RenderFile(filePath)
		{
		}
	};
}