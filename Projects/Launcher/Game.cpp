#include <Engine.h>
#include "DebugWorld.h"

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
	Game()
	{
		PushLayer(new ExampleLayer());
		GetWorldManager().Add(new DebugWorld());
	}

	~Game()
	{

	}

};

Engine::Application* Engine::CreateApplication()
{
	return new Game();
}