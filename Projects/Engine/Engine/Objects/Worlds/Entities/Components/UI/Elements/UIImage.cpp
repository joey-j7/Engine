#include "pch.h"
#include "UIImage.h"

#include "Platform/FileLoader.h"

namespace Engine
{
	UIImage::UIImage(Entity& Entity, const std::string& Text, const std::string& sName) : UIElement(Entity, sName)
	{
		SetImage(Text);
	}
	
	UIImage::UIImage(Entity& Entity, sk_sp<SkImage>* Image, const std::string& sName) : UIElement(Entity, sName)
	{
		m_ImageReference = Image;
		
		MarkDirty();
		m_Loaded = true;
	}

	void UIImage::SetImage(const std::string& Path)
	{
		if (m_Path == Path)
			return;

		MarkDirty();
		
		m_Path = Path;
		m_Loaded = false;

		uint32_t Length = 0;
		char* Data = FileLoader::Read(m_Path, Length, FileLoader::Type::E_CONTENT);

		if (Length == 0 || !Data)
			return;

		// Clear old image
		if (m_Image.get())
		{
			m_Image.reset();
		}

		m_Image = SkImage::MakeFromEncoded(SkData::MakeFromMalloc(Data, Length));

		m_Image = SkImage::MakeFromEncoded(SkData::MakeFromMalloc(Data, Length));

		if (m_Image.get())
		{
			MeasureSize();
			m_Loaded = true;
		}
	}

	void UIImage::SetLinearFiltering(bool Filter)
	{
		if (m_UseLinearFiltering == Filter)
			return;

		m_UseLinearFiltering = Filter;
		MarkDirty();
	}

	void UIImage::BeginDraw()
	{
		// Modify as reference might change every frame
		if (m_ImageReference && m_ImageReference->get() && m_ImageDimensions == Vector2(0.f))
		{
			m_ImageDimensions.x = m_ImageReference->get()->width();
			m_ImageDimensions.y = m_ImageReference->get()->height();
		}

		if (m_ImageDimensions != Vector2(0.f) &&
			m_ImageDimensions != Vector2(m_Width, m_Height))
		{
			GetTransform()->SetScale(m_Width / m_ImageDimensions.x, m_Height / m_ImageDimensions.y);
		}
		
		UIElement::BeginDraw();
	}

	void UIImage::Draw()
	{
		if (!m_Loaded)
			return;
		
		sk_sp<SkImage> Image = m_Image;

		if (m_ImageReference)
			Image = *m_ImageReference;

		if (!Image.get())
			return;
		
		m_Canvas->drawImage(Image, 0, 0, SkSamplingOptions(
			m_UseLinearFiltering ? SkFilterMode::kLinear : SkFilterMode::kNearest
		));
	}

	void UIImage::MeasureSize()
	{
		m_ImageDimensions = Vector2(
			m_Image->width(),
			m_Image->height()
		);
		
		SetWidth(static_cast<uint32_t>(m_ImageDimensions.x));
		SetHeight(static_cast<uint32_t>(m_ImageDimensions.y));
	}
}
