#include <Engine.h>
#include "Engine/Events/KeyEvent.h"

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

	void Call(Engine::Event& event) override
	{
		if (event.GetEventType() == Engine::EventType::KeyPressed)
		{
			Engine::KeyPressedEvent& e = (Engine::KeyPressedEvent&)event;
			if (e.GetKeyCode() == CB_KEY_TAB)
				CB_TRACE("Tab key is pressed (event)!");
			CB_TRACE("Character \"{0}\" pressed!", (char)e.GetKeyCode());
		}
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