#include "pch.h"
#include "DebugWorld.h"

#include "Engine/Application.h"

#include "Engine/Objects/Worlds/Entities/StaticEntity.h"

#include "Engine/Objects/Worlds/Entities/Components/Transform/Transform2DComponent.h"
#include "Engine/Objects/Worlds/Entities/Components/Transform/Transform3DComponent.h"
#include "Engine/Objects/Worlds/Entities/Components/ClickableComponent.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Elements/UIImage.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Elements/UIText.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Elements/Shapes/UILine.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Elements/Shapes/UIOval.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Elements/Shapes/UIRect.h"
#include "Engine/Objects/Worlds/Entities/UI/UIButton.h"
#include "Engine/Objects/Worlds/Entities/UI/Camera/UICameraPreview.h"

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
	
	// StaticEntity* Entity = new StaticEntity("Test Entity");
	// Entity->AddComponent<Transform2DComponent>()->SetPosition(Vector2(100.f, 0.f));
	// Entity->AddComponent<UIText>("Zico bruh")->SetColor(SK_ColorGREEN);
	// Entity->AddComponent<ClickableComponent>();

	// StaticEntity* Entity2 = new StaticEntity("Test Rect");
	// Entity2->AddComponent<Transform2DComponent>();// ->SetPosition(Vector2(10.f, 10.f));
	// Entity2->AddComponent<ClickableComponent>();
	// UIRect* Rect = Entity2->AddComponent<UIRect>();
	// Rect->SetColor(SK_ColorRED);
	// Rect->SetRadius(10);
	// Rect->SetBorder(5, UIElement::Gradient({ { 0, SK_ColorBLACK }, { 1, SK_ColorGREEN } }));
	
	// StaticEntity* Entity3 = new StaticEntity("Test Entity 2");
	// Entity3->AddComponent<Transform2DComponent>()->SetPosition(Vector2(100.f, 50.f));
	// Entity3->AddComponent<ClickableComponent>();
	// UIText* Text = Entity3->AddComponent<UIText>("Alles goed jonge");
	// Text->SetFontSize(32);
	// Text->SetBorder(1, UIElement::Gradient(
	// 	{ { 0, SK_ColorYELLOW }, { 1, SK_ColorMAGENTA } },
	// 	UIElement::Gradient::E_VERTICAL),
	// 	false
	// );

	UICameraPreview* CameraPreview = new UICameraPreview();
	CameraPreview->GetComponent<UIImage>()->SetShader(
		"uniform shader Element;"
		"half4 main(float2 Coord) {"
		"  return sample(Element, Coord).rgba;"  // Sample 'input', then swap red and blue
		"}"
	);
	
	UIButton* Button = new UIButton(
		[]() {
			Application::Get().GetHardwareContext().GetCamera().TakePhoto();
		},
		{
			"Capture"
		},
		{
			"Hover",
			Color(1.f, 1.f, 0.f)
		},
		{
			"Press",
			Color(1.f, 0.f, 0.f)
		}
	);

	/*StaticEntity* Line = new StaticEntity("Line");
	Line->AddComponent<UILine>();
	
	UIButton* Oval1 = new UIButton("", nullptr);
	Oval1->SetParent(Line);
	UIOval* OvalC1 = Oval1->AddComponent<UIOval>();
	OvalC1->SetWidth(100);
	OvalC1->SetHeight(100);
	
	UIButton* Oval2 = new UIButton("", nullptr);
	Oval2->SetParent(Line);
	UIOval* OvalC2 = Oval1->AddComponent<UIOval>();
	OvalC2->SetWidth(100);
	OvalC2->SetHeight(100);

	UIButton* Oval3 = new UIButton("", nullptr);
	Oval3->SetParent(Line);
	UIOval* OvalC3 = Oval1->AddComponent<UIOval>();
	OvalC3->SetWidth(100);
	OvalC3->SetHeight(100);

	UIButton* Oval4 = new UIButton("", nullptr);
	Oval4->SetParent(Line);
	UIOval* OvalC4 = Oval1->AddComponent<UIOval>();
	OvalC4->SetWidth(100);
	OvalC4->SetHeight(100);*/
	
	// StaticEntity* Entity4 = new StaticEntity("Button");
	// Entity4->AddComponent<ClickableComponent>();
	// Entity4->AddComponent<UIImage>("image.png");
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