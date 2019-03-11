#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "ResourceManagerBase.h"
#include "ResourceType.h"

namespace Engine
{
	template<class T>
	class ResourceManager : public ResourceManagerBase
	{
		static_assert(std::is_base_of<ResourceType, T>::value, "Type must be a descendant of ResourceType");

	public:
		ResourceManager(const std::vector<std::string>& supportedExtensions) { m_SupportedExtensions = supportedExtensions; };
		~ResourceManager();

		virtual ResourceType* Add(const std::string& filePath) override;
		virtual bool Remove(ResourceType* resource) override;

		void SetFunctions(std::function<void(T*)> load, std::function<void(T*)> unload)
		{
			m_LoadFn = load;
			m_UnloadFn = unload;
		}

	protected:
		std::unordered_map<std::string, T*> m_Resources;

		std::function<void(T*)> m_LoadFn;
		std::function<void(T*)> m_UnloadFn;
	};

	template<class T>
	ResourceManager<T>::~ResourceManager()
	{
		for (auto& obj : m_Resources)
		{
			m_UnloadFn(obj.second);
			delete obj.second;
		}
	}

	template<class T>
	ResourceType* ResourceManager<T>::Add(const std::string& filePath)
	{
		auto it = m_Resources.find(filePath);

		if (it == m_Resources.end())
		{
			T* resource = new T(filePath);
			resource->Add();

			m_LoadFn(resource);

			m_Resources.emplace(filePath, resource);
			return resource;
		}

		it->second->Add();
		return it->second;
	}

	template<class T>
	bool ResourceManager<T>::Remove(ResourceType* resource)
	{
		auto it = m_Resources.find(resource->GetFilePath());

		if (it != m_Resources.end())
		{
			it->second->Remove();

			if (!it->second->HasReferences())
			{
				m_UnloadFn(it->second);

				delete it->second;
				m_Resources.erase(it);

				return true;
			}
		}

		return false;
	}
}