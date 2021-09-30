#include "pch.h"

#include "Views/PhotoView.h"
#include "PhotoEntity.h"

#include "Engine/Objects/Worlds/Entities/Components/UI/Renderables/UIImage.h"

PhotoEntity::PhotoEntity(const String& FilePath, const String& Name) : Engine::StaticEntity(Name)
{
	StaticEntity* CameraPreview = new StaticEntity();
	Photo = CameraPreview->AddComponent<UIImage>();

	uint32_t Length = 0;
	char* File = FileLoader::Read(FilePath, Length, FileLoader::E_ROOT);

	Photo->SetImageData(File, Length);
	delete[] File;

	Photo->ScaleWithDPI(false);
	Photo->SetPivot(Vector2(0.5f));
	Photo->SetAnchor(E_ANCH_CENTER);

	Window& Window = Application::Get().GetRenderContext().GetWindow();

	Vector2 Dims = Photo->GetDimensions();
	const float Width = Window.GetWidth() / Dims.x;
	const float Height = Window.GetHeight() / Dims.y;

	const float Scale = glm::min(Width, Height);

	Photo->SetScale(Vector2(Scale, Scale));
}
