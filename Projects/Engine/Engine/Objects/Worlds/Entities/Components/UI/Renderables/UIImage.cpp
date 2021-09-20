#include "pch.h"
#include "UIImage.h"

#include "Platform/FileLoader.h"

namespace Engine
{
	UIImage::UIImage(Entity& Entity, const String& Text, const String& sName) : UIRenderable(Entity, sName)
	{
		SetImage(Text);
	}

	UIImage::~UIImage()
	{

	}
	
	void UIImage::SetImage(const String& Path)
	{
		if (m_Path == Path)
			return;

		m_Mutex.lock();

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

			SetElementShader(
				m_Image->makeShader(SkSamplingOptions(m_UseLinearFiltering ? SkFilterMode::kLinear : SkFilterMode::kNearest))
			);
			
			m_Loaded = true;
		}

		m_Mutex.unlock();
	}
	
	void UIImage::SetImageData(char* Data, size_t Length)
	{
		m_Mutex.lock();

		m_Path = "";

		m_Image = SkImage::MakeFromEncoded(SkData::MakeWithoutCopy(Data, Length));
		
		if (m_Image.get())
		{
			MeasureSize();
			
			SetElementShader(
				m_Image->makeShader(SkSamplingOptions(m_UseLinearFiltering ? SkFilterMode::kLinear : SkFilterMode::kNearest))
			);
			
			m_Loaded = true;

			// Event callback
			OnImageDataReceived();
		}
		
		MarkDirty();

		m_Mutex.unlock();
	}

	void UIImage::SetLinearFiltering(bool Filter)
	{
		if (m_UseLinearFiltering == Filter)
			return;

		m_UseLinearFiltering = Filter;

		if (m_Image.get())
		{
			SetElementShader(
				m_Image->makeShader(SkSamplingOptions(m_UseLinearFiltering ? SkFilterMode::kLinear : SkFilterMode::kNearest))
			);
		}
		
		MarkDirty();
	}
	
	void UIImage::Draw()
	{
		if (!m_Loaded)
			return;

		m_Mutex.lock();

		if (!m_Image.get())
		{
			m_Mutex.unlock();
			return;
		}

		if (m_ImageDimensions != Vector2(0.f) &&
			m_ImageDimensions != Vector2(m_Width, m_Height))
		{
			const float Width = m_Width / m_ImageDimensions.x;
			const float Height = m_Height / m_ImageDimensions.y;
			const float Scale = glm::min(Width, Height);

			// m_Canvas->scale(Scale, Scale);
			m_Canvas->scale(m_Width / m_ImageDimensions.x, m_Height / m_ImageDimensions.y);
		}

		if (!m_RuntimeShaderInfo.m_Shader.get())
		{
			m_Canvas->drawImage(m_Image, 0, 0, SkSamplingOptions(
				m_UseLinearFiltering ? SkFilterMode::kLinear : SkFilterMode::kNearest
			), &m_Paint);
		}
		// Image is included within paint
		else
		{
			m_Canvas->drawPaint(m_Paint);
		}

		m_Mutex.unlock();
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
