#include "pch.h"
#include "UIImage.h"

#include "Platform/FileLoader.h"

namespace Engine
{
	UIImage::UIImage(Entity& Entity, const std::string& Text, const std::string& sName) : UIElement(Entity, sName)
	{
		SetImage(Text);
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
	
	void UIImage::Draw()
	{
		if (!m_Loaded)
			return;

		m_Canvas->scale(
			m_Width / m_ImageDimensions.x,
			m_Height / m_ImageDimensions.y
		);

		m_Canvas->drawImage(m_Image, 0, 0, SkSamplingOptions(
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
