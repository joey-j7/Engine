#include "pch.h"

#include "ResourceDatabase.h"

namespace Engine
{
	ResourceType* ResourceDatabase::Load(const std::string& filePath, FileLoader::Type pathType)
	{
		std::string extension = FileLoader::GetExtension(filePath);

		for (auto& manager : m_Managers)
		{
			ResourceManager<ResourceType>* resourceManager = manager.second;

			if (resourceManager->Supports(extension))
				return resourceManager->Add(FileLoader::GetPath(filePath, pathType));
		}

		CB_CORE_ERROR("There are no resource managers with support for type {0], file: {1}", extension, filePath);
		return nullptr;
	}
}