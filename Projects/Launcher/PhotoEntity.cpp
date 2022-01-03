#include "pch.h"
#include "PhotoEntity.h"

#include "Views/PhotoView.h"

#include "Engine/Objects/Components/UI/Renderables/UIImage.h"

PhotoEntity::PhotoEntity(Engine::World& World, const String& m_FilePath, const String& Name) : Engine::Entity(World, Name)
{
	m_Photo = AddComponent<Engine::UIImage>();
	m_Photo->SetImage(m_FilePath, Engine::FileLoader::E_ROOT);

	m_Photo->ScaleWithDPI(false);
	m_Photo->SetPivot(Engine::Vector2(0.5f));
	m_Photo->SetAnchor(Engine::E_ANCH_CENTER);

	Engine::Window& Window = Engine::Application::Get().GetRenderContext().GetWindow();

	Engine::Vector2 Dims = m_Photo->GetDimensions();
	const float Width = Window.GetWidth() / Dims.x;
	const float Height = Window.GetHeight() / Dims.y;

	const float Scale = glm::min(Width, Height);

	m_Photo->SetScale(Engine::Vector2(Scale, Scale));
}
