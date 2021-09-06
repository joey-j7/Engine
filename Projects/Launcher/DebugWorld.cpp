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
#include "Engine/Objects/Worlds/Entities/Components/UI/Elements/Shapes/UIRect.h"
#include "Engine/Objects/Worlds/Entities/UI/UIButton.h"
#include "Engine/Objects/Worlds/Entities/UI/Camera/UICameraPreview.h"

using namespace Engine;

DebugWorld::DebugWorld()
{
	UICameraPreview* CameraPreview = new UICameraPreview();

	CameraImage = CameraPreview->GetComponent<UIImage>();
	CameraImage->SetShader(
		"uniform shader Element;"
		"uniform float2 screenSize;"

		"uniform float2 top0;"
		"uniform float2 btm0;"
		"uniform float2 top1;"
		"uniform float2 btm1;"

		"half4 main(float2 Coord) {"
		"  float2 Coords = sk_FragCoord.xy;"
		"  float2 ImgCoords = Coord.xy;"

		"  float m = (top0.y - btm0.y) / (top0.x - btm0.x);"
		"  float c = top0.y - m * top0.x;"
		"  float2 line1 = float2((Coords.y - c) / m, Coords.x * m + c);"

		"  m = (top1.y - btm1.y) / (top1.x - btm1.x);"
		"  c = top1.y - m * top1.x;"
		"  float2 line2 = float2((Coords.y - c) / m, Coords.x * m + c);"

		"  if ((Coords.x >= line1.x && top0.y >= btm0.y) || "
		"      (Coords.x < line1.x && top0.y < btm0.y))"
		"    ImgCoords.y += (Coords.x - line1.x) * 1.5;"

		"  else if ((Coords.x < line2.x && top0.y >= btm0.y) || "
		"      (Coords.x >= line2.x && top0.y < btm0.y))"
		"    ImgCoords.y += (Coords.x - line2.x) * 1.5;"

		"  half4 col = sample(Element, ImgCoords.xy).rgba;"
		"  return col;" 
		"}"
	);

	CameraImage->OnImageDataReceived.Bind(this, &DebugWorld::OnCameraImageData);

	constexpr float BtnSize = 40.f;

	UIButton* Button = new UIButton(
		{ "", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f) },
		{ "", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 1.f, 0.f) },
		{ "", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 0.f, 0.f) },
		"Camera Button"
	);

	Button->SetOnClickedCallback([]() {
		Application::Get().GetHardwareContext().GetCamera().TakePhoto();
	});

	// Line 1
	LineEntity = new StaticEntity("Line");
	Line = LineEntity->AddComponent<UILine>();

	Line->SetStartPosition(Vector2(0.f, 0.f));
	Line->SetEndPosition(Vector2(0.f, 400.f));
	Line->SetColor(Color(1.f, 0.f, 0.f));
	Line->SetThickness(10);

	constexpr float OvalSize = 40.f;

	Oval1 = new UIButton(
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 1.f, 0.f) },
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 0.f, 0.f) },
		"Start Button"
	);
	
	Oval1->SetParent(LineEntity);
	Oval1->SetAnchor(E_ANCH_TOP_LEFT);

	Oval2 = new UIButton(
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f)},
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 1.f, 0.f) },
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 0.f, 0.f) },
		"End Button"
	);
	
	Oval2->SetParent(LineEntity);
	Oval2->SetAnchor(E_ANCH_TOP_LEFT);

	LineEntity->GetComponent<Transform2DComponent>()->SetPosition(
		Vector2(150.f, 100.f)
	);

	Oval2->GetComponent<Transform2DComponent>()->SetPosition(
		Line->GetEndPosition(),
		true
	);

	Oval1->SetOnDraggedCallback([&](const DVector2& Delta)
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

		UpdateCameraUniforms();
	});

	Oval2->SetOnDraggedCallback([&](const DVector2& Delta)
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

		UpdateCameraUniforms();
	});

	
	// Line 2
	LineEntity2 = new StaticEntity("Line");
	Line2 = LineEntity2->AddComponent<UILine>();

	Line2->SetStartPosition(Vector2(0.f, 0.f));
	Line2->SetEndPosition(Vector2(0.f, 400.f));
	Line2->SetColor(Color(0.f, 0.f, 1.f));
	Line2->SetThickness(10);
	
	Oval3 = new UIButton(
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 1.f, 0.f) },
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 0.f, 0.f) },
		"Start Button"
	);
	
	Oval3->SetParent(LineEntity2);
	Oval3->SetAnchor(E_ANCH_TOP_LEFT);

	Oval4 = new UIButton(
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f)},
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 1.f, 0.f) },
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 0.f, 0.f) },
		"End Button"
	);
	
	Oval4->SetParent(LineEntity2);
	Oval4->SetAnchor(E_ANCH_TOP_LEFT);

	LineEntity2->GetComponent<Transform2DComponent>()->SetPosition(
		Vector2(250.f, 100.f)
	);

	Oval2->GetComponent<Transform2DComponent>()->SetPosition(
		Line->GetEndPosition(),
		true
	);

	Oval3->SetOnDraggedCallback([&](const DVector2& Delta)
	{
		if (!Window::IsMouseInView())
			return;
		
		Window& Window = Application::Get().GetRenderContext().GetWindow();

		const float InvScale = 1.f / Window.GetScale();
		const float WinWidth = Window.GetWidth() * InvScale;
		const float WinHeight = Window.GetHeight() * InvScale;

		const float Width = static_cast<float>(Oval4->GetComponent<UIRect>()->GetWidth());
		const float Height = static_cast<float>(Oval4->GetComponent<UIRect>()->GetHeight());

		Vector2 New = Line2->GetStartPosition() + Delta * InvScale;
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
		
		auto Transform = LineEntity2->GetComponent<Transform2DComponent>();

		Vector2 DeltaNew = New - Transform->GetPosition();
		Transform->SetPosition(New);

		Line2->SetEndPosition(
			Line2->GetEndPosition() - DeltaNew
		);

		Oval4->GetComponent<Transform2DComponent>()->SetPosition(
			Line2->GetEndPosition(),
			true
		);

		UpdateCameraUniforms();
	});

	Oval4->SetOnDraggedCallback([&](const DVector2& Delta)
	{
		if (!Window::IsMouseInView())
			return;
		
		Window& Window = Application::Get().GetRenderContext().GetWindow();
		
		const float InvScale = 1.f / Window.GetScale();
		const float WinWidth = Window.GetWidth() * InvScale;
		const float WinHeight = Window.GetHeight() * InvScale;

		const float Width = static_cast<float>(Oval4->GetComponent<UIRect>()->GetWidth());
		const float Height = static_cast<float>(Oval4->GetComponent<UIRect>()->GetHeight());

		const Vector2& Start = Line2->GetStartPosition();

		Vector2 New = Line2->GetEndPosition() + Delta * InvScale;
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
		
		Line2->SetEndPosition(New);

		Oval4->GetComponent<Transform2DComponent>()->SetPosition(
			Line2->GetEndPosition(),
			true
		);

		UpdateCameraUniforms();
	});
}

void DebugWorld::UpdateCameraUniforms()
{
	Window& Window = Application::Get().GetRenderContext().GetWindow();
	
	const float Scale = Window.GetScale();

	// Line 1
	Vector3 Start = Oval1->GetComponent<Transform2DComponent>()->GetPosition() * Scale;
	Vector3 End = Oval2->GetComponent<Transform2DComponent>()->GetPosition() * Scale;
	
	SkV2 Top = SkV2{ Start.x, Start.y };
	CameraImage->SetShaderUniform("top0", Top);

	SkV2 Btm = SkV2{ End.x, End.y };
	CameraImage->SetShaderUniform("btm0", Btm);

	// Line 2
	Vector3 Start1 = Oval3->GetComponent<Transform2DComponent>()->GetPosition() * Scale;
	Vector3 End1 = Oval4->GetComponent<Transform2DComponent>()->GetPosition() * Scale;

	SkV2 Top1 = SkV2{ Start1.x, Start1.y };
	CameraImage->SetShaderUniform("top1", Top1);

	SkV2 Btm1 = SkV2{ End1.x, End1.y };
	CameraImage->SetShaderUniform("btm1", Btm1);

	const float Width = CameraImage->GetWidth();
	const float Height = CameraImage->GetHeight();

	CameraImage->SetShaderUniform("screenSize", SkV2 { Width, Height });
}

void DebugWorld::Draw(float fDeltaTime)
{
	// Draw layers and objects
	World::Draw(fDeltaTime);
}

void DebugWorld::OnCameraImageData()
{
	Window& Window = Application::Get().GetRenderContext().GetWindow();
	Vector2 Dims = CameraImage->GetDimensions();

	const Camera& Camera = Application::Get().GetHardwareContext().GetCamera();

	if (Camera.GetOrientation() == 90 || Camera.GetOrientation() == 270)
	{
		Vector2 BakDims = Dims;

		Dims.x = Dims.y;
		Dims.y = BakDims.x;
	}

	const float Width = Window.GetWidth() / Dims.x;
	const float Height = Window.GetHeight() / Dims.y;

	CameraImage->SetScale(Vector2(Width, Height));
}