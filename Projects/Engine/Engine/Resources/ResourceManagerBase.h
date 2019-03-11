#pragma once

namespace Engine
{
	class ResourceType;

	class ResourceManagerBase
	{
	public:
		virtual ~ResourceManagerBase() {};

		virtual ResourceType* Add(const std::string& filePath) = 0;
		virtual bool Remove(const std::string& filePath) = 0;

		bool Supports(const std::string& extension)
		{
			return std::find(m_SupportedExtensions.begin(), m_SupportedExtensions.end(), extension) != m_SupportedExtensions.end();
		}

	protected:
		std::vector<std::string> m_SupportedExtensions;
	};
}