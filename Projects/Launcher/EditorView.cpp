#include "pch.h"
#include "EditorView.h"

#include "Engine/Application.h"

#include "Engine/Objects/Worlds/Entities/StaticEntity.h"

#include "Engine/Objects/Worlds/Entities/Components/Transform/Transform2DComponent.h"
#include "Engine/Objects/Worlds/Entities/Components/Transform/Transform3DComponent.h"
#include "Engine/Objects/Worlds/Entities/Components/ClickableComponent.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Renderables/UIImage.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Renderables/UIText.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Renderables/Shapes/UILine.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Renderables/Shapes/UIRect.h"
#include "Engine/Objects/Worlds/Entities/UI/UIButton.h"
#include "Engine/Objects/Worlds/Entities/UI/Camera/UICameraPreview.h"

using namespace Engine;

EditorView::EditorView()
{
	StaticEntity* CameraPreview = new StaticEntity();
	CameraImage = CameraPreview->AddComponent<UIImage>();

	/*
	
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

	*/

	constexpr float BtnSize = 40.f;

	UIButton* Button = new UIButton(
		{ "", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f) },
		{ "", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 1.f, 0.f) },
		{ "", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 0.f, 0.f) },
		"Save Button"
	);

	Button->SetAnchor(E_ANCH_BOTTOM);
	Button->SetPivot(Vector2(0.5f, 0.5f));

	Button->GetComponent<Transform2DComponent>()->Translate(Vector2(0.f, -100.f));

	Button->SetOnClickedCallback([]() {
		Application::Get().GetHardwareContext().GetCamera().TakePhoto();
	});

	// Window
	Window& Window = Application::Get().GetRenderContext().GetWindow();
	float InvScale = 1.f / Window.GetScale();

	// Line 1
	LineEntity1 = new StaticEntity("Line");
	Line1 = LineEntity1->AddComponent<UILine>();

	Line1->SetStartPosition(Vector2(0.f, 0.f));
	Line1->SetEndPosition(Vector2(0.f, Window.GetHeight() * 0.8f * InvScale));
	Line1->SetColor(Color(1.f, 0.f, 0.f));
	Line1->SetThickness(10);

	constexpr float OvalSize = 40.f;

	StartOval1 = new UIButton(
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 1.f, 0.f) },
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 0.f, 0.f) },
		"Start Button"
	);

	StartOval1->SetParent(LineEntity1);
	StartOval1->SetAnchor(E_ANCH_TOP_LEFT);
	StartOval1->SetPivot(Vector2(0.5f, 0.5f));

	EndOval1 = new UIButton(
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 1.f, 0.f) },
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 0.f, 0.f) },
		"End Button"
	);

	EndOval1->SetParent(LineEntity1);
	EndOval1->SetAnchor(E_ANCH_TOP_LEFT);
	EndOval1->SetPivot(Vector2(0.5f, 0.5f));

	LineEntity1->GetComponent<Transform2DComponent>()->SetPosition(
		Vector2(Window.GetWidth() * 0.4f * InvScale, Window.GetHeight() * 0.1f * InvScale)
	);

	EndOval1->GetComponent<Transform2DComponent>()->SetPosition(
		Line1->GetEndPosition(),
		true
	);

	StartOval1->SetOnDraggedCallback([&](const DVector2& Delta)
	{
		SetStartPosition(Delta, *Line1, *EndOval1);
	});

	EndOval1->SetOnDraggedCallback([&](const DVector2& Delta)
	{
		SetEndPosition(Delta, *Line1, *EndOval1);
	});


	// Line 2
	LineEntity2 = new StaticEntity("Line");
	Line2 = LineEntity2->AddComponent<UILine>();

	Line2->SetStartPosition(Vector2(0.f, 0.f));
	Line2->SetEndPosition(Vector2(0.f, Window.GetHeight() * 0.8f * InvScale));
	Line2->SetColor(Color(0.f, 0.f, 1.f));
	Line2->SetThickness(10);

	StartOval2 = new UIButton(
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 1.f, 0.f) },
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 0.f, 0.f) },
		"Start Button"
	);

	StartOval2->SetParent(LineEntity2);
	StartOval2->SetAnchor(E_ANCH_TOP_LEFT);
	StartOval2->SetPivot(Vector2(0.5f, 0.5f));

	EndOval2 = new UIButton(
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 1.f, 0.f) },
		{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 0.f, 0.f) },
		"End Button"
	);

	EndOval2->SetParent(LineEntity2);
	EndOval2->SetAnchor(E_ANCH_TOP_LEFT);
	EndOval2->SetPivot(Vector2(0.5f, 0.5f));

	LineEntity2->GetComponent<Transform2DComponent>()->SetPosition(
		Vector2(Window.GetWidth() * 0.6f * InvScale, Window.GetHeight() * 0.1f * InvScale)
	);

	EndOval2->GetComponent<Transform2DComponent>()->SetPosition(
		Line2->GetEndPosition(),
		true
	);

	StartOval2->SetOnDraggedCallback([&](const DVector2& Delta)
	{
		SetStartPosition(Delta, *Line2, *EndOval2);
	});

	EndOval2->SetOnDraggedCallback([&](const DVector2& Delta)
	{
		SetEndPosition(Delta, *Line2, *EndOval2);
	});
}

void EditorView::UpdateCameraUniforms()
{
	Window& Window = Application::Get().GetRenderContext().GetWindow();

	const float Scale = Window.GetScale();

	// Line 1
	Vector3 Start = StartOval1->GetComponent<Transform2DComponent>()->GetPosition() * Scale;
	Vector3 End = EndOval1->GetComponent<Transform2DComponent>()->GetPosition() * Scale;

	SkV2 Top = SkV2{ Start.x, Start.y };
	CameraImage->SetShaderUniform("top0", Top);

	SkV2 Btm = SkV2{ End.x, End.y };
	CameraImage->SetShaderUniform("btm0", Btm);

	// Line 2
	Vector3 Start1 = StartOval2->GetComponent<Transform2DComponent>()->GetPosition() * Scale;
	Vector3 End1 = EndOval2->GetComponent<Transform2DComponent>()->GetPosition() * Scale;

	SkV2 Top1 = SkV2{ Start1.x, Start1.y };
	CameraImage->SetShaderUniform("top1", Top1);

	SkV2 Btm1 = SkV2{ End1.x, End1.y };
	CameraImage->SetShaderUniform("btm1", Btm1);

	const float Width = CameraImage->GetWidth();
	const float Height = CameraImage->GetHeight();

	CameraImage->SetShaderUniform("screenSize", SkV2{ Width, Height });
}

void EditorView::Draw(float fDeltaTime)
{
	// Draw layers and objects
	World::Draw(fDeltaTime);
}

void EditorView::SetStartPosition(const Engine::DVector2& Delta, Engine::UILine& Line, Engine::UIButton& EndOval)
{
	if (!Window::IsMouseInView())
		return;

	Window& Window = Application::Get().GetRenderContext().GetWindow();

	const float InvScale = 1.f / Window.GetScale();
	const float WinWidth = Window.GetWidth() * InvScale;
	const float WinHeight = Window.GetHeight() * InvScale;

	AABB Bounds = EndOval.GetBounds();

	const float Width = Bounds.width();
	const float Height = Bounds.height();

	Vector2 New = Line.GetStartPosition() + Delta * InvScale;
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

	auto Transform = Line.GetEntity().GetComponent<Transform2DComponent>();

	Vector2 DeltaNew = New - Transform->GetPosition();
	Transform->SetPosition(New);

	Line.SetEndPosition(
		Line.GetEndPosition() - DeltaNew
	);

	EndOval.GetComponent<Transform2DComponent>()->SetPosition(
		Line.GetEndPosition(),
		true
	);

	UpdateCameraUniforms();
}

void EditorView::SetEndPosition(const DVector2& Delta, Engine::UILine& Line, Engine::UIButton& EndOval)
{
	if (!Window::IsMouseInView())
		return;

	Window& Window = Application::Get().GetRenderContext().GetWindow();

	const float InvScale = 1.f / Window.GetScale();
	const float WinWidth = Window.GetWidth() * InvScale;
	const float WinHeight = Window.GetHeight() * InvScale;

	AABB Bounds = EndOval.GetBounds();

	const float Width = Bounds.width();
	const float Height = Bounds.height();

	const Vector2& Start = Line.GetStartPosition();

	Vector2 New = Line.GetEndPosition() + Delta * InvScale;
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

	Line.SetEndPosition(New);

	EndOval.GetComponent<Transform2DComponent>()->SetPosition(
		Line.GetEndPosition(),
		true
	);

	UpdateCameraUniforms();
}

void EditorView::OnCameraImageData()
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