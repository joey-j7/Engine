#include "pch.h"

#include "FileDatabase.h"

namespace Engine
{
	FileDatabase* FileDatabase::s_Instance = nullptr;
	
	FileDatabase::FileDatabase()
	{
		CB_CORE_ASSERT(!s_Instance, "File Database already exists!");
		s_Instance = this;
	};

	FileDatabase::~FileDatabase()
	{
		for (auto& manager : m_Managers)
		{
			delete manager.second;
		}

		if (s_Instance == this)
			s_Instance = nullptr;
	}

	FileReference* FileDatabase::Load(const std::string& filePath, FileLoader::Type pathType)
	{
		const std::string extension = FileLoader::GetExtension(filePath);
		
		if (!FileLoader::Exists(filePath, pathType))
		{
			CB_CORE_WARN("No file found at path \"{0]\"", filePath);
			return nullptr;
		}

		for (auto& manager : m_Managers)
		{
			FileManagerBase* resourceManager = manager.second;

			if (resourceManager->Supports(extension))
			{
				return resourceManager->Add(FileLoader::GetPath(filePath, pathType));
			}
		}

		CB_CORE_WARN("There are no resource managers with support for type \"{0]\", file: \"{1}\"", extension, filePath);
		return nullptr;
	}

	bool FileDatabase::Unload(FileReference* resource)
	{
		const std::string& extension = resource->GetExtension();

		for (auto& manager : m_Managers)
		{
			FileManagerBase* resourceManager = manager.second;

			if (resourceManager->Supports(extension))
			{
				return resourceManager->Remove(resource);
			}
		}

		return false;
	}

}