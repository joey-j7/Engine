#pragma once

#include "UIElement.h"

#include <include/core/SkImage.h>

namespace Engine
{
	class Engine_API UIImage : public UIElement
	{
	public:
		UIImage(Entity& Entity, const std::string& ImagePath = "", const std::string& sName = "Image");
		UIImage(Entity& Entity, sk_sp<SkImage>* Image, const std::string& sName = "Image");

		const std::string& GetPath() const { return m_Path; }
		void SetImage(const std::string& Path);

		bool GetLinearFiltering() const { return m_UseLinearFiltering; }
		void SetLinearFiltering(bool Filter);

		virtual void BeginDraw() override;
		virtual void Draw() override;
		
	private:
		void MeasureSize();
		
		std::string m_Path = "";
		bool m_Loaded = false;

		bool m_UseLinearFiltering = true;

		sk_sp<SkImage> m_Image;
		sk_sp<SkImage>* m_ImageReference = nullptr;
		Vector2 m_ImageDimensions;
	};

	Engine_REGISTER_COMP(UIImage);
}