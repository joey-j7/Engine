#include "pch.h"
#include "Renderer2D.h"

#include <include/gpu/gl/GrGLInterface.h>
#include <glad/glad.h>

#include "Engine/Application.h"

namespace Engine
{
	Renderer2D::Renderer2D()
	{

	}

	Renderer2D::~Renderer2D()
	{

	}

	void Renderer2D::Present()
	{
		if (m_Components.size() == 0)
			return;

		// Clear background
		m_Canvas->drawColor(SK_ColorWHITE);

		for (auto& Component : m_Components)
		{
			Component->BeginDraw();
			Component->Draw();
			Component->EndDraw();
		}

		m_Surface->flushAndSubmit();
		
		/*
		SkPaint paint;
		paint.setColor(SK_ColorRED);

		m_Canvas->save();

		const float scale = Window.GetScale();
		m_Canvas->scale(scale, scale);

		// Draw a rectangle with red paint
		const float t = static_cast<float>(DeltaTime::GetTime());

		paint.setAntiAlias(true);
		const SkScalar r = 0.f + glm::abs(glm::sin(t)) * 30;
		SkScalar radii[] = { r, r, r, r, r, r, r, r };
		SkMatrix translation;

		float a = 360.f;
		translation.postRotate(glm::mod(t * 90.f, a), 100, 100);

		SkPath path;
		path.addRoundRect({ 50, 50, 150, 150 }, radii);
		path.transform(translation);

		m_Canvas->drawPath(path, paint);

		// Draw a message with a nice black paint
		SkFont font;
		font.setSubpixel(true);
		font.setSize(60);
		paint.setColor(SK_ColorBLACK);

		static const char message[] = "Hello World\0";

		// Translate and rotate
		m_Canvas->translate(Window.GetWidth() / scale * 0.5f, Window.GetHeight() / scale * 0.5f);

		if (!Image)
		{
			uint32_t length = 0;
			char* data = FileLoader::Read("image.png", length, FileLoader::Type::E_CONTENT);
			Image = SkImage::MakeFromEncoded(SkData::MakeFromMalloc(data, length));
		}

		if (Image)
		{
			m_Canvas->drawImage(Image, 0, 0, SkSamplingOptions(SkFilterMode::kLinear));
		}

		// Draw the text
		m_Canvas->drawString(message, 0, 0, font, paint);

		m_Canvas->restore();
		m_Canvas->save();

		m_Canvas->scale(scale, scale);

		// Draw the text
		font.setSize(16);

		const std::string fps = std::to_string(DeltaTime::GetFPS()) + "FPS " + std::to_string(DeltaTime::Get()) + "ms " + std::to_string(scale) + "scale";
		m_Canvas->drawString(fps.c_str(), 50, 100, font, paint);

		m_Canvas->restore();
		*/
	}
}
