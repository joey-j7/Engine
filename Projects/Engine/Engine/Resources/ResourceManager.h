#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "ResourceType.h"

namespace Engine
{
	class ResourceType;

	template<class T>
	class ResourceManager
	{
		static_assert(std::is_base_of<ResourceType, T>::value, "Type must be a descendant of ResourceType");
	public:
		ResourceManager(const std::vector<std::string>& supportedExtensions) { m_SupportedExtensions = supportedExtensions; };
		~ResourceManager();

		T* Add(const std::string& filePath);
		bool Remove(const std::string& filePath);

		bool Supports(const std::string& extension)
		{
			return m_Resources.find(extension) != m_Resources.end();
		}

	protected:
		std::unordered_map<std::string, T*> m_Resources;
		std::vector<std::string> m_SupportedExtensions;
	};

	template<class T>
	ResourceManager<T>::~ResourceManager()
	{
		for (auto& obj : m_Resources)
		{
			delete obj.second;
		}
	}

	template<class T>
	T* ResourceManager<T>::Add(const std::string& filePath)
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
	bool ResourceManager<T>::Remove(const std::string& filePath)
	{
		auto it = m_Resources.find(filePath);

		if (it != m_Resources.end())
		{
			it.second->m_ReferenceCount--;

			if (it.second->m_ReferenceCount == 0)
			{
				delete it.second;
				m_Resources.erase(it);

				return true;
			}
		}

		return false;
	}
}