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
	UICameraPreview* CameraPreview = new UICameraPreview();
	UIImage* CameraImage = CameraPreview->GetComponent<UIImage>();
	CameraImage->SetShader(
		"uniform shader Element;"
		"uniform float2 screenSize;"
		"uniform float2 top0;"
		"uniform float2 btm0;"
		"uniform float2 top1;"
		"uniform float2 btm1;"
		"  "
		"half4 main(float2 Coord) {"
		"  float2 Intersection = Coord;"
		"  "
		"  float Slope = (top0.y - btm0.y) / (top0.x - btm0.x);"
		"  float YIntercept = top0.y - top0.x * Slope;"
		"  Intersection.y = (Intersection.x - YIntercept) / Slope;"
		"  "
		"  if (Coord.y <= screenSize.y * 0.5 - Intersection.y) Coord.y = screenSize.y - Coord.y; "
		//"  Coord.y = screenSize.y - Coord.y;"
		"  return sample(Element, Coord).rgba;"  // Sample 'input', then swap red and blue
		"}"
	);

	UIButton* Button = new UIButton(
		{ "", 20.f, Vector2(20.f), "", "", Color(1.f) },
		{ "", 20.f, Vector2(20.f), "", "", Color(1.f, 1.f, 0.f) },
		{ "", 20.f, Vector2(20.f), "", "", Color(1.f, 0.f, 0.f) },
		"Camera Button"
	);

	Button->SetOnClickedCallback([]() {
		Application::Get().GetHardwareContext().GetCamera().TakePhoto();
	});

	StaticEntity* LineEntity = new StaticEntity("Line");
	UILine* Line = LineEntity->AddComponent<UILine>();

	Line->SetStartPosition(Vector2(0.f, 0.f));
	Line->SetEndPosition(Vector2(100.f, 100.f));
	Line->SetColor(Color(1.f, 0.f, 0.f));
	Line->SetThickness(10);
	
	UIButton* Oval1 = new UIButton(
		{ "", 10.f, Vector2(10.f), "", "", Color(1.f) },
		{ "", 10.f, Vector2(10.f), "", "", Color(1.f, 1.f, 0.f) },
		{ "", 10.f, Vector2(10.f), "", "", Color(1.f, 0.f, 0.f) },
		"Start Button"
	);
	
	Oval1->SetParent(LineEntity);
	Oval1->SetAnchor(E_ANCH_TOP_LEFT);

	UIButton* Oval2 = new UIButton(
		{ "", 10.f, Vector2(10.f), "", "", Color(1.f)},
		{ "", 10.f, Vector2(10.f), "", "", Color(1.f, 1.f, 0.f) },
		{ "", 10.f, Vector2(10.f), "", "", Color(1.f, 0.f, 0.f) },
		"End Button"
	);
	
	Oval2->SetParent(LineEntity);
	Oval2->SetAnchor(E_ANCH_TOP_LEFT);

	LineEntity->GetComponent<Transform2DComponent>()->SetPosition(
		Vector2(300, 100)
	);

	Oval2->GetComponent<Transform2DComponent>()->SetPosition(
		Line->GetEndPosition(),
		true
	);

	Oval1->SetOnDraggedCallback([LineEntity, Line, Oval2, CameraImage](const DVector2& Delta)
	{
		if (!Window::IsMouseInView())
			return;
		
		Window& Window = Application::Get().GetRenderContext().GetWindow();

		const float InvScale = 1.f / Window.GetScale();
		const float WinWidth = Window.GetWidth() * InvScale;
		const float WinHeight = Window.GetHeight() * InvScale;

		const float Width = static_cast<float>(Oval2->GetComponent<UIRect>()->GetWidth());
		const float Height = static_cast<float>(Oval2->GetComponent<UIRect>()->GetHeight());

		Vector2 New = Line->GetStartPosition() + Delta * InvScale;
		New.x = glm::min(
			glm::max(
				New.x, Width * 0.5f
			), WinWidth - Width * 0.5f
		);

		New.y = glm::min(
			glm::max(
				New.y, Height * 0.5f
			), WinHeight - Height * 0.5f
		);
		
		auto Transform = LineEntity->GetComponent<Transform2DComponent>();

		Vector2 DeltaNew = New - Transform->GetPosition();
		Transform->SetPosition(New);

		Line->SetEndPosition(
			Line->GetEndPosition() - DeltaNew
		);

		Oval2->GetComponent<Transform2DComponent>()->SetPosition(
			Line->GetEndPosition(),
			true
		);

		Vector2 End = New + Line->GetEndPosition();

		SkV2 Uniform = SkV2{ New.x, New.y };
		CameraImage->SetShaderUniform("top0", Uniform);

		SkV2 Uniform2 = SkV2{ End.x, End.y };
		CameraImage->SetShaderUniform("btm0", Uniform2);

		SkV2 Size = SkV2{ WinWidth, WinHeight };
		CameraImage->SetShaderUniform("screenSize", Size);
	});

	Oval2->SetOnDraggedCallback([Line, Oval2, CameraImage](const DVector2& Delta)
	{
		if (!Window::IsMouseInView())
			return;
		
		Window& Window = Application::Get().GetRenderContext().GetWindow();
		
		const float InvScale = 1.f / Window.GetScale();
		const float WinWidth = Window.GetWidth() * InvScale;
		const float WinHeight = Window.GetHeight() * InvScale;

		const float Width = static_cast<float>(Oval2->GetComponent<UIRect>()->GetWidth());
		const float Height = static_cast<float>(Oval2->GetComponent<UIRect>()->GetHeight());

		const Vector2& Start = Line->GetStartPosition();

		Vector2 New = Line->GetEndPosition() + Delta * InvScale;
		New.x = glm::min(
			glm::max(
				New.x, Width * 0.5f - Start.x
			), WinWidth - Width * 0.5f - Start.x
		);

		New.y = glm::min(
			glm::max(
				New.y, Height * 0.5f - Start.y
			), WinHeight - Height * 0.5f - Start.y
		);
		
		Line->SetEndPosition(New);

		Oval2->GetComponent<Transform2DComponent>()->SetPosition(
			Line->GetEndPosition(),
			true
		);

		SkV2 Uniform = SkV2{ (Start.x + New.x) * Window.GetScale(), (Start.y + New.y) * Window.GetScale() };
		CameraImage->SetShaderUniform("top0", Uniform);

		SkV2 Uniform2 = SkV2{ Start.x * Window.GetScale(), Start.y * Window.GetScale() };
		CameraImage->SetShaderUniform("btm0", Uniform2);

		SkV2 Size = SkV2{ WinWidth, WinHeight };
		CameraImage->SetShaderUniform("screenSize", Size);
	});
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