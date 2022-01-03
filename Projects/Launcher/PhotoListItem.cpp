#include "pch.h"
#include "PhotoListItem.h"

#include "Engine/Objects/Components/UI/Renderables/UIImage.h"

PhotoListItem::PhotoListItem(
	World& World,
	const String& Path,
	const ButtonStyle& DefaultStyle,
	const ButtonStyle& HoverStyle,
	const ButtonStyle& PressStyle
) : UIButton(World, DefaultStyle, HoverStyle, PressStyle, "")
{
	m_FilePath = Path;

	Window& Window = Application::Get().GetRenderContext().GetWindow();
	const float Width = Window.GetWidth();

	m_Dimensions = Vector2(Width * 0.23f);

	Entity* PhotoEntity = GetWorld().Add<Entity>("Photo");
	PhotoEntity->SetParent(this);

	// Photo thumbnail
	m_Photo = PhotoEntity->AddComponent<Engine::UIImage>();

	m_Photo->SetPivot(Engine::Vector2(0.5f));
	m_Photo->SetAnchor(Engine::E_ANCH_CENTER);

	m_Photo->ScaleWithDPI(false);

	// Load thumbnail image
	// std::thread(&PhotoListItem::LoadImage, this).detach();
	LoadImage();
}

void PhotoListItem::LoadImage()
{
	// Check if thumbnail cache exists
	bool Exists = FileLoader::Exists(
		".cache/thumbs/" + FileLoader::GetName(m_FilePath) + ".jpg",
		FileLoader::Type::E_EXTERNAL
	);

	if (Exists)
	{
		m_Photo->SetImage(
			".cache/thumbs/" + FileLoader::GetName(m_FilePath) + ".jpg",
			FileLoader::Type::E_EXTERNAL
		);

		m_Thumbnail = m_Photo->GetImage();

		return;
	}

	// Set to original image so we can make a thumbnail out of it
	m_Photo->SetImage(m_FilePath, Engine::FileLoader::E_ROOT);

	m_Thumbnail = MakeThumbnail();

	m_Photo->SetImage(
		".cache/thumbs/" + FileLoader::GetName(m_FilePath) + ".jpg",
		FileLoader::Type::E_EXTERNAL
	);
}

sk_sp<SkImage> PhotoListItem::MakeThumbnail()
{
	const SkImageInfo Info = SkImageInfo::MakeN32(m_Dimensions.x, m_Dimensions.y, kOpaque_SkAlphaType);
	GrDirectContext* Context = Application::Get().GetRenderContext().GetAPI().GetRenderer2D()->GetContext();

	sk_sp<SkSurface> Surface = SkSurface::MakeRenderTarget(Context, SkBudgeted::kNo, Info);

	SkCanvas* Canvas = Surface->getCanvas();
	// Canvas->clear(SK_ColorBLACK);

	// Paint.setColor(SK_ColorGREEN);

	Vector2 Dimensions = Vector2(m_Photo->GetWidth(), m_Photo->GetHeight());
	int32_t Min = glm::min(Dimensions.x, Dimensions.y);

	SkRect SrcRect = SkRect::MakeXYWH(
		Dimensions.x * 0.5f - Min * 0.5f,
		Dimensions.y * 0.5f - Min * 0.5f,
		Min,
		Min
	);

	SkRect DestRect = SkRect::MakeXYWH(0.f, 0.f, m_Dimensions.x, m_Dimensions.y);

	Canvas->drawImageRect(
		m_Photo->GetImage(),
		SrcRect,
		DestRect,
		SkSamplingOptions(SkFilterMode::kLinear),
		&m_Photo->GetPaint(),
		SkCanvas::SrcRectConstraint::kFast_SrcRectConstraint
	);

	sk_sp<SkImage> Image = Surface->makeImageSnapshot();
	sk_sp<SkData> JPG(Image->encodeToData(SkEncodedImageFormat::kJPEG, 80));

	FileLoader::Write(
		".cache/thumbs/",
		FileLoader::GetName(m_FilePath) + ".jpg",
		(char*)JPG->data(),
		JPG->size(),
		true,
		FileLoader::Type::E_EXTERNAL
	);

	return Image;
}
