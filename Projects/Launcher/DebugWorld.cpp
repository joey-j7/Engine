#include "pch.h"
#include "DebugWorld.h"

#include "Engine/Application.h"

#include "Engine/Objects/Worlds/Entities/StaticEntity.h"

#include "Engine/Objects/Worlds/Entities/Components/Transform/TransformComponent2D.h"
#include "Engine/Objects/Worlds/Entities/Components/Transform/TransformComponent3D.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/UIClickable.h"

#include "Engine/Objects/Worlds/Entities/Components/UI/UIText.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Shapes/UIRect.h"

DebugWorld::DebugWorld()
{
	// Engine::CommandEngine* pEngine = Engine::Application::Get().GetRenderContext().GetAPI().GetCommandEngine("Screen");
	// 
	// Engine::ShaderProgram::Descriptor shaderDesc;
	// shaderDesc.Vertex = "default.vert";
	// shaderDesc.Pixel = "default.frag";
	// 
	// m_pPass = pEngine->Create(shaderDesc);
	//
	// Engine::Application::Get().GetHardwareContext().GetCamera().Start();

	Engine::StaticEntity* Entity = new Engine::StaticEntity("Test Entity");
	Entity->AddComponent<Engine::TransformComponent2D>()->SetPosition(Vector2(100.f, 0.f));
	Entity->AddComponent<Engine::UIText>("Zico bruh")->SetColor(SK_ColorGREEN);

	Engine::StaticEntity* Entity2 = new Engine::StaticEntity("Test Rect");
	Entity2->AddComponent<Engine::TransformComponent2D>()->SetPosition(Vector2(10.f, 10.f));
	Engine::UIRect* Rect = Entity2->AddComponent<Engine::UIRect>();
	Rect->SetColor(SK_ColorRED);
	Rect->SetRadius(10);
	Rect->SetBorder(5, Engine::UIComponent::Gradient({ { 0, SK_ColorBLACK }, { 1, SK_ColorGREEN } }));
	
	Engine::StaticEntity* Entity3 = new Engine::StaticEntity("Test Entity 2");
	Entity3->AddComponent<Engine::TransformComponent2D>()->SetPosition(Vector2(100.f, 50.f));
	Engine::UIText* Text = Entity3->AddComponent<Engine::UIText>("Alles goed jonge");
	Text->SetFontSize(32);
	Text->SetBorder(1, Engine::UIComponent::Gradient(
		{ { 0, SK_ColorYELLOW }, { 1, SK_ColorMAGENTA } },
		Engine::UIComponent::Gradient::E_VERTICAL),
		false
	);

	Engine::StaticEntity* Entity4 = new Engine::StaticEntity("Button");
	Entity3->AddComponent<Engine::UIClickable>();
}

void DebugWorld::Draw(float fDeltaTime)
{
	// Draw layers and objects
	World::Draw(fDeltaTime);

	// m_pPass->Begin();
	// m_pPass->Clear();
	// m_pPass->Draw();
	// m_pPass->End();
}
