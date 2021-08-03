#pragma once

#include "Engine/Files/Rendering/RenderFile.h"

namespace Engine
{
	class Engine_API ModelFile : public RenderFile
	{
	public:
		ModelFile(const String& filePath) : RenderFile(filePath)
		{
		}
	};
}
