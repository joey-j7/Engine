#pragma once

extern Engine::Application* Engine::CreateApplication();

int main(int argc, char** argv)
{
	Engine::Log::Init();
	CB_CORE_INFO_T(5.0f, "Engine initialized on platform {0}", TOSTRING(CB_PLATFORM));
	
	auto app = Engine::CreateApplication();
	app->Run();
	delete app;

	return 0;
}