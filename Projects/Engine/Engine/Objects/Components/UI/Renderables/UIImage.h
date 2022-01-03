#pragma once

#include "UIRenderable.h"

#include <include/core/SkImage.h>

namespace Engine
{
	class Engine_API UIImage : public UIRenderable
	{
	public:
		UIImage(Entity& Entity, const String& ImagePath = "", const String& sName = "Image");
		~UIImage();
		
		const String& GetPath() const { return m_Path; }

		void SetImage(const String& Path, const FileLoader::Type Type = FileLoader::Type::E_CONTENT);
		void SetSkImage(sk_sp<SkImage> Image, const String& Path = "");
		void SetImageData(char* Data, size_t Length);

		sk_sp<SkImage> GetImage() const { return m_Image; }

		const Vector2& GetDimensions() const { return m_ImageDimensions; }

		bool GetLinearFiltering() const { return m_UseLinearFiltering; }
		void SetLinearFiltering(bool Filter);
		
		virtual void Draw() override;

		Event<void> OnImageDataReceived;

	private:
		void MeasureSize();
		
		String m_Path = "";
		bool m_Loaded = false;

		bool m_UseLinearFiltering = true;

		sk_sp<SkImage> m_Image;
		Vector2 m_ImageDimensions;

		std::mutex m_Mutex;
	};
}