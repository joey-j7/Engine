#include "pch.h"

#include "ResourceDatabase.h"

namespace Engine
{
	ResourceDatabase::~ResourceDatabase()
	{
		for (auto& manager : m_Managers)
		{
			delete manager.second;
		}
	}

	ResourceType* ResourceDatabase::Load(const std::string& filePath, FileLoader::Type pathType)
	{
		const std::string extension = FileLoader::GetExtension(filePath);
		
		if (!FileLoader::Exists(filePath, pathType))
		{
			CB_CORE_WARN("No file found at path \"{0]\"", filePath);
			return nullptr;
		}

		for (auto& manager : m_Managers)
		{
			ResourceManagerBase* resourceManager = manager.second;

			if (resourceManager->Supports(extension))
			{
				return resourceManager->Add(FileLoader::GetPath(filePath, pathType));
			}
		}

		CB_CORE_WARN("There are no resource managers with support for type \"{0]\", file: \"{1}\"", extension, filePath);
		return nullptr;
	}

	bool ResourceDatabase::Unload(ResourceType* resource)
	{
		const std::string& extension = resource->GetExtension();

		for (auto& manager : m_Managers)
		{
			ResourceManagerBase* resourceManager = manager.second;

			if (resourceManager->Supports(extension))
			{
				return resourceManager->Remove(resource);
			}
		}

		return false;
	}

}