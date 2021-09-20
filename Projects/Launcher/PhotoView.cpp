#include "pch.h"
#include "PhotoView.h"

#include "EditorView.h"
#include "ListView.h"

#include "Engine/Objects/Worlds/Entities/UI/UIButton.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Renderables/UIImage.h"

using namespace Engine;

PhotoView::PhotoView(const String& FilePath)
{
	StaticEntity* CameraPreview = new StaticEntity();
	CameraImage = CameraPreview->AddComponent<UIImage>();

	uint32_t Length = 0;
	char* File = FileLoader::Read(FilePath, Length, FileLoader::E_INTERNAL);

	CameraImage->SetImageData(File, Length);
	delete[] File;

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

	//constexpr float BtnSize = 40.f;

	//UIButton* Button = new UIButton(
	//	{ "", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f) },
	//	{ "", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 1.f, 0.f) },
	//	{ "", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 0.f, 0.f) },
	//	"Save Button"
	//);

	//Button->SetAnchor(E_ANCH_BOTTOM);
	//Button->SetPivot(Vector2(0.5f, 0.5f));

	//Button->GetComponent<Transform2DComponent>()->Translate(Vector2(0.f, -100.f));

	//Button->SetOnClickedCallback([]() {
	//	Application::Get().GetHardwareContext().GetCamera().TakePhoto();
	//});

	//// Window
	//Window& Window = Application::Get().GetRenderContext().GetWindow();
	//float InvScale = 1.f / Window.GetScale();

	//// Line 1
	//LineEntity1 = new StaticEntity("Line");
	//Line1 = LineEntity1->AddComponent<UILine>();

	//Line1->SetStartPosition(Vector2(0.f, 0.f));
	//Line1->SetEndPosition(Vector2(0.f, Window.GetHeight() * 0.8f * InvScale));
	//Line1->SetColor(Color(1.f, 0.f, 0.f));
	//Line1->SetThickness(10);

	//constexpr float OvalSize = 40.f;

	//StartOval1 = new UIButton(
	//	{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
	//	{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 1.f, 0.f) },
	//	{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 0.f, 0.f) },
	//	"Start Button"
	//);

	//StartOval1->SetParent(LineEntity1);
	//StartOval1->SetAnchor(E_ANCH_TOP_LEFT);
	//StartOval1->SetPivot(Vector2(0.5f, 0.5f));

	//EndOval1 = new UIButton(
	//	{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
	//	{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 1.f, 0.f) },
	//	{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 0.f, 0.f) },
	//	"End Button"
	//);

	//EndOval1->SetParent(LineEntity1);
	//EndOval1->SetAnchor(E_ANCH_TOP_LEFT);
	//EndOval1->SetPivot(Vector2(0.5f, 0.5f));

	//LineEntity1->GetComponent<Transform2DComponent>()->SetPosition(
	//	Vector2(Window.GetWidth() * 0.4f * InvScale, Window.GetHeight() * 0.1f * InvScale)
	//);

	//EndOval1->GetComponent<Transform2DComponent>()->SetPosition(
	//	Line1->GetEndPosition(),
	//	true
	//);

	//StartOval1->SetOnDraggedCallback([&](const DVector2& Delta)
	//{
	//	SetStartPosition(Delta, *Line1, *EndOval1);
	//});

	//EndOval1->SetOnDraggedCallback([&](const DVector2& Delta)
	//{
	//	SetEndPosition(Delta, *Line1, *EndOval1);
	//});

	//// Line 2
	//LineEntity2 = new StaticEntity("Line");
	//Line2 = LineEntity2->AddComponent<UILine>();

	//Line2->SetStartPosition(Vector2(0.f, 0.f));
	//Line2->SetEndPosition(Vector2(0.f, Window.GetHeight() * 0.8f * InvScale));
	//Line2->SetColor(Color(0.f, 0.f, 1.f));
	//Line2->SetThickness(10);

	//StartOval2 = new UIButton(
	//	{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
	//	{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 1.f, 0.f) },
	//	{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 0.f, 0.f) },
	//	"Start Button"
	//);

	//StartOval2->SetParent(LineEntity2);
	//StartOval2->SetAnchor(E_ANCH_TOP_LEFT);
	//StartOval2->SetPivot(Vector2(0.5f, 0.5f));

	//EndOval2 = new UIButton(
	//	{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
	//	{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 1.f, 0.f) },
	//	{ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 0.f, 0.f) },
	//	"End Button"
	//);

	//EndOval2->SetParent(LineEntity2);
	//EndOval2->SetAnchor(E_ANCH_TOP_LEFT);
	//EndOval2->SetPivot(Vector2(0.5f, 0.5f));

	//LineEntity2->GetComponent<Transform2DComponent>()->SetPosition(
	//	Vector2(Window.GetWidth() * 0.6f * InvScale, Window.GetHeight() * 0.1f * InvScale)
	//);

	//EndOval2->GetComponent<Transform2DComponent>()->SetPosition(
	//	Line2->GetEndPosition(),
	//	true
	//);

	//StartOval2->SetOnDraggedCallback([&](const DVector2& Delta)
	//{
	//	SetStartPosition(Delta, *Line2, *EndOval2);
	//});

	//EndOval2->SetOnDraggedCallback([&](const DVector2& Delta)
	//{
	//	SetEndPosition(Delta, *Line2, *EndOval2);
	//});

	// Back button
	constexpr float BtnSize = 40.f;

	UIButton* BackButton = new UIButton(
		{ "", BtnSize * 2.f, Vector4(BtnSize * 0.66f), "", "", Color(1.f) },
		{ "", BtnSize * 2.f, Vector4(BtnSize * 0.66f), "", "", Color(1.f, 1.f, 0.f) },
		{ "", BtnSize * 2.f, Vector4(BtnSize * 0.66f), "", "", Color(1.f, 0.f, 0.f) },
		"Back Button"
	);

	BackButton->SetAnchor(E_ANCH_TOP_LEFT);
	BackButton->SetPivot(Vector2(0.0f, 0.0f));

	BackButton->GetComponent<Transform2DComponent>()->Translate(Vector2(25.f, 25.f));

	BackButton->SetOnClickedCallback([&]() {
		Application::Get().ThreadedCallback.Bind(this, &PhotoView::OnListView);
	});
}

void PhotoView::OnEditorView()
{
	Application::Get().GetWorldManager().Remove(
		Application::Get().GetWorldManager().GetActive()
	);

	EditorView* View = new EditorView();
}

void PhotoView::OnListView()
{
	Application::Get().GetWorldManager().Remove(
		Application::Get().GetWorldManager().GetActive()
	);

	ListView* View = new ListView();
}
