#pragma once

extern Engine::Application* Engine::CreateApplication();
Engine::Application* app = nullptr;

#ifdef CB_PLATFORM_WINDOWS
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#else
int main(int argc, char** argv)
#endif
{
	if (!app)
	{
		CB_CORE_INFO("Engine initialized on platform {0}", TOSTRING(CB_PLATFORM));
		app = Engine::CreateApplication();
	}

	app->Run();

	delete app;
	app = nullptr;

	return 0;
}