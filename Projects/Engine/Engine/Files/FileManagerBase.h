#pragma once

namespace Engine
{
	class FileReference;

	class FileManagerBase
	{
		friend class FileDatabase;

	public:
		virtual ~FileManagerBase() {};

	protected:
		virtual FileReference* Add(const std::string& filePath) = 0;
		virtual bool Remove(FileReference* resource) = 0;

		bool Supports(const std::string& extension)
		{
			return std::find(m_SupportedExtensions.begin(), m_SupportedExtensions.end(), extension) != m_SupportedExtensions.end();
		}

		std::vector<std::string> m_SupportedExtensions;
	};
}