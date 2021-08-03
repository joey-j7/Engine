#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "FileManagerBase.h"
#include "FileReference.h"

namespace Engine
{
	template<class T>
	class FileManager : public FileManagerBase
	{
		static_assert(std::is_base_of<FileReference, T>::value, "Type must be a descendant of FileReference");

	public:
		FileManager(const std::vector<String>& supportedExtensions) { m_SupportedExtensions = supportedExtensions; };
		~FileManager();

		virtual FileReference* Add(const String& filePath) override;
		virtual bool Remove(FileReference* resource) override;

	protected:
		std::unordered_map<String, T*> m_Resources;
	};

	template<class T>
	FileManager<T>::~FileManager()
	{
		for (auto& obj : m_Resources)
		{
			delete obj.second;
		}
	}

	template<class T>
	FileReference* FileManager<T>::Add(const String& filePath)
	{
		auto it = m_Resources.find(filePath);

		if (it == m_Resources.end())
		{
			T* resource = new T(filePath);
			resource->Add();

			m_Resources.emplace(filePath, resource);
			return resource;
		}

		it->second->Add();
		return it->second;
	}

	template<class T>
	bool FileManager<T>::Remove(FileReference* resource)
	{
		auto it = m_Resources.find(resource->GetFilePath());

		if (it != m_Resources.end())
		{
			it->second->Remove();

			if (!it->second->HasReferences())
			{
				delete it->second;
				m_Resources.erase(it);

				return true;
			}
		}

		return false;
	}
}