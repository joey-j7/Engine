#include <Engine.h>

#include "Views/CameraView.h"

class Game : public Engine::Application
{
public:
	Game() : Application(TOSTRING(CB_NAME))
	{
		Engine::WorldManager::Get().Push<CameraView>();
	}

	~Game()
	{

	}
};

Engine::Application* Engine::CreateApplication()
{
	return new Game();
}