#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Platform/FileLoader.h"

#include "Engine/Resources/ResourceManager.h"
#include "Engine/Resources/ResourceType.h"

#include "Rendering/Resources/ModelResource.h"
#include "Rendering/Resources/TextureResource.h"
#include "Rendering/Resources/ShaderResource.h"

namespace Engine
{
	class Engine_API ResourceDatabase
	{
	public:
		ResourceDatabase() { };

		template <typename T>
		ResourceManager<T>* Add(const std::vector<std::string>& supportedExtensions);

		template <typename T>
		ResourceManager<T>* Get();

		ResourceType* Load(const std::string& filePath, FileLoader::Type pathType);

	protected:
		std::unordered_map<size_t, ResourceManager<ResourceType>*> m_Managers;
	};

	template <typename T>
	ResourceManager<T>* ResourceDatabase::Add(const std::vector<std::string>& supportedExtensions)
	{
		static_assert(std::is_base_of<ResourceType, T>::value, "Type must be a descendant of ResourceType");

		ResourceManager<T>* find = Get<T>();
		if (!find)
		{
			ResourceManager<T>* t = new ResourceManager<T>(supportedExtensions);
			m_Managers.emplace(typeid(T).hash_code(), reinterpret_cast<ResourceManager<ResourceType>*>(t));
			return t;
		}

		return find;
	}

	template <typename T>
	ResourceManager<T>* ResourceDatabase::Get()
	{
		static_assert(std::is_base_of<ResourceType, T>::value, "Type must be a descendant of ResourceType");

		auto f = m_Managers.find(typeid(T).hash_code());
		if (f != m_Managers.end())
			return reinterpret_cast<ResourceManager<T>*>(f->second);

		return nullptr;
	}
}