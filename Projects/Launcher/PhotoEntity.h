#pragma once

namespace Engine
{
	class UIImage;
}

class PhotoEntity : public Engine::Entity
{
public:
	PhotoEntity(Engine::World& World, const String& m_FilePath, const String& Name = "Photo Entity");

	Engine::UIImage* GetPhoto() const { return m_Photo; }

protected:
	Engine::UIImage* m_Photo = nullptr;
};