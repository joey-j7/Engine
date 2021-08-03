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
		virtual FileReference* Add(const String& filePath) = 0;
		virtual bool Remove(FileReference* resource) = 0;

		bool Supports(const String& extension)
		{
			return std::find(m_SupportedExtensions.begin(), m_SupportedExtensions.end(), extension) != m_SupportedExtensions.end();
		}

		std::vector<String> m_SupportedExtensions;
	};
}