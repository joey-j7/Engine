#include "pch.h"
#include "PhotoEntity.h"

#include "Views/PhotoView.h"

#include "Engine/Objects/Worlds/Entities/Components/UI/Renderables/UIImage.h"

PhotoEntity::PhotoEntity(const String& FilePath, const String& Name) : Engine::StaticEntity(Name)
{
	Photo = AddComponent<Engine::UIImage>();

	uint32_t Length = 0;
	char* File = Engine::FileLoader::Read(FilePath, Length, Engine::FileLoader::E_ROOT);

	Photo->SetImageData(File, Length);
	delete[] File;

	Photo->ScaleWithDPI(false);
	Photo->SetPivot(Engine::Vector2(0.5f));
	Photo->SetAnchor(Engine::E_ANCH_CENTER);

	Engine::Window& Window = Engine::Application::Get().GetRenderContext().GetWindow();

	Engine::Vector2 Dims = Photo->GetDimensions();
	const float Width = Window.GetWidth() / Dims.x;
	const float Height = Window.GetHeight() / Dims.y;

	const float Scale = glm::min(Width, Height);

	Photo->SetScale(Engine::Vector2(Scale, Scale));
}
