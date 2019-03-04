#pragma once

#include <vector>
#include <unordered_map>

class ResourceType;

template<class T>
class ResourceManager
{
	static_assert(std::is_base_of<ResourceType, T>::value, "Type must be a descendant of ResourceType");
public:
	~ResourceManager();

	T* Add(const std::string& filePath);
	bool Remove(const std::string& filePath);

protected:
	std::unordered_map<std::string, T*> m_Resources;
};