#include "pch.h"
#include "DebugWorld.h"

#include "Engine/Application.h"

#include "Engine/Objects/Worlds/Entities/StaticEntity.h"

#include "Engine/Objects/Worlds/Entities/Components/Transform/TransformComponent2D.h"
#include "Engine/Objects/Worlds/Entities/Components/Transform/TransformComponent3D.h"
#include "Engine/Objects/Worlds/Entities/Components/ClickableComponent.h"

#include "Engine/Objects/Worlds/Entities/Components/UI/UIText.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Shapes/UIRect.h"

using namespace Engine;

DebugWorld::DebugWorld()
{
	// CommandEngine* pEngine = Application::Get().GetRenderContext().GetAPI().GetCommandEngine("Screen");
	// 
	// ShaderProgram::Descriptor shaderDesc;
	// shaderDesc.Vertex = "default.vert";
	// shaderDesc.Pixel = "default.frag";
	// 
	// m_pPass = pEngine->Create(shaderDesc);
	//
	// Application::Get().GetHardwareContext().GetCamera().Start();

	StaticEntity* Entity = new StaticEntity("Test Entity");
	Entity->AddComponent<TransformComponent2D>()->SetPosition(Vector2(100.f, 0.f));
	Entity->AddComponent<UIText>("Zico bruh")->SetColor(SK_ColorGREEN);
	Entity->AddComponent<ClickableComponent>();

	StaticEntity* Entity2 = new StaticEntity("Test Rect");
	Entity2->AddComponent<TransformComponent2D>()->SetPosition(Vector2(10.f, 10.f));
	Entity2->AddComponent<ClickableComponent>();
	UIRect* Rect = Entity2->AddComponent<UIRect>();
	Rect->SetColor(SK_ColorRED);
	Rect->SetRadius(10);
	Rect->SetBorder(5, UIComponent::Gradient({ { 0, SK_ColorBLACK }, { 1, SK_ColorGREEN } }));
	
	StaticEntity* Entity3 = new StaticEntity("Test Entity 2");
	Entity3->AddComponent<TransformComponent2D>()->SetPosition(Vector2(100.f, 50.f));
	Entity3->AddComponent<ClickableComponent>();
	UIText* Text = Entity3->AddComponent<UIText>("Alles goed jonge");
	Text->SetFontSize(32);
	Text->SetBorder(1, UIComponent::Gradient(
		{ { 0, SK_ColorYELLOW }, { 1, SK_ColorMAGENTA } },
		UIComponent::Gradient::E_VERTICAL),
		false
	);

	StaticEntity* Entity4 = new StaticEntity("Button");
	Entity4->AddComponent<ClickableComponent>();
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
