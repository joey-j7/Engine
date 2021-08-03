#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Platform/FileLoader.h"

#include "Engine/Files/FileManagerBase.h"
#include "Engine/Files/FileManager.h"
#include "Engine/Files/FileReference.h"

#include "Engine/Files/Rendering/ModelFile.h"
#include "Engine/Files/Rendering/TextureFile.h"
#include "Engine/Files/Rendering/ShaderFile.h"

namespace Engine
{
	class Engine_API FileDatabase
	{
	public:
		FileDatabase();
		~FileDatabase();

		template <typename T>
		FileManager<T>* Add(const std::vector<String>& supportedExtensions);

		template <typename T>
		FileManager<T>* Get();

		static FileDatabase& Get() { return *s_Instance; }

		FileReference* Load(const String& filePath, FileLoader::Type pathType);
		bool Unload(FileReference* resource);

	protected:
		static FileDatabase* s_Instance;
		std::unordered_map<size_t, FileManagerBase*> m_Managers;
	};

	template <typename T>
	FileManager<T>* FileDatabase::Add(const std::vector<String>& supportedExtensions)
	{
		static_assert(std::is_base_of<FileReference, T>::value, "Type must be a descendant of FileReference");

		FileManager<T>* find = Get<T>();
		if (!find)
		{
			FileManager<T>* t = new FileManager<T>(supportedExtensions);
			m_Managers.emplace(typeid(T).hash_code(), reinterpret_cast<FileManager<FileReference>*>(t));
			return t;
		}

		return find;
	}

	template <typename T>
	FileManager<T>* FileDatabase::Get()
	{
		static_assert(std::is_base_of<FileReference, T>::value, "Type must be a descendant of FileReference");

		auto f = m_Managers.find(typeid(T).hash_code());
		if (f != m_Managers.end())
			return reinterpret_cast<FileManager<T>*>(f->second);

		return nullptr;
	}
}