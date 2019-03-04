#pragma once

extern Engine::Application* Engine::CreateApplication();
Engine::Application* app = nullptr;

int main(int argc, char** argv)
{
	if (!app)
	{
		Engine::Log::Init();
		CB_CORE_INFO_T(5.0f, "Engine initialized on platform {0}", TOSTRING(CB_PLATFORM));

		app = Engine::CreateApplication();
	}

	app->Run();

	delete app;
	app = nullptr;

	return 0;
}