#pragma once

#include "Engine/Files/FileReference.h"

namespace Engine
{
	class Engine_API RenderFile : public FileReference
	{
	public:
		RenderFile(const std::string & filePath) : FileReference(filePath)
		{
		};
	};
}