#include "ResourceManager.h"

template<class T>
ResourceManager<T>::~ResourceManager()
{
	for (T& obj : m_Resources)
	{
		delete obj;
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