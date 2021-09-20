#include <Engine.h>

#include "ListView.h"
#include "CameraView.h"

class ExampleLayer : public Engine::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void Update(float fDeltaTime) override
	{
		if (Engine::Input::IsKeyPressed(CB_KEY_TAB))
			CB_TRACE("Tab key is pressed (poll)!");
	}
};

class Game : public Engine::Application
{
public:
	Game() : Application("The Nest")
	{
		PushLayer(new ExampleLayer());
		ListView* View = new ListView();
	}

	~Game()
	{

	}
};

Engine::Application* Engine::CreateApplication()
{
	return new Game();
}