#pragma once

#include <vector>
#include <unordered_map>

class ResourceType;

template<class T>
class ResourceManager
{
	static_assert(std::is_base_of<ResourceType, T>::value, "Type must be a descendant of ResourceType");
public:
	ResourceManager() {};
	~ResourceManager();

	T* Add(const std::string& filePath);
	bool Remove(const std::string& filePath);

protected:
	std::unordered_map<std::string, T*> m_Resources;
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
		resource->m_ReferenceCount++;

		m_Resources.emplace(filePath, resource);
		return resource;
	}

	it.second->m_ReferenceCount++;
	return it.second;
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