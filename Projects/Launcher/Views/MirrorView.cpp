#include "pch.h"
#include "MirrorView.h"

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

#include "../PhotoEntity.h"

#include "CameraView.h"
#include "ListView.h"

using namespace Engine;

MirrorView::MirrorView(const String& FilePath, const String& Name) : SubView(FilePath, Name)
{
	ClickableComponent* Clickable = m_Photo->AddComponent<ClickableComponent>();
	Clickable->OnClickedEvent.Bind(this, &MirrorView::ToggleButtons);

	// Camera
	m_CameraImage->SetShader(
		"uniform shader Element;"

		"uniform float2 imageSize;"
		"uniform float2 screenSize;"

		"uniform float2 top0;"
		"uniform float2 btm0;"
		"uniform float2 top1;"
		"uniform float2 btm1;"

		"uniform int mirrorCount;"

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
		"      (Coords.x < line1.x && top0.y < btm0.y)) {"
		"      if (mirrorCount >= 1) "
		"        ImgCoords.x = line1.x * 2.0 / screenSize.x * imageSize.x - ImgCoords.x; }"

		"  else if ((Coords.x < line2.x && top0.y >= btm0.y) || "
		"      (Coords.x >= line2.x && top0.y < btm0.y)) {"
		"      if (mirrorCount >= 2) "
		"        ImgCoords.x = line2.x * 2.0 / screenSize.x * imageSize.x - ImgCoords.x; }"

		"  if (ImgCoords.x < 0.0 || ImgCoords.x > imageSize.x ||"
		"		ImgCoords.y < 0.0 || ImgCoords.y > imageSize.y)"
		"  return half4(0.0, 0.0, 0.0, 1.0);"

		"  return Element.eval(ImgCoords.xy);"
		"}"
	);

	constexpr float BtnSize = 50.f;

	// Next Button
	NextButton = new UIButton(
		{ "V", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f) },
		{ "V", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 1.f, 0.f) },
		{ "V", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 0.f, 0.f) },
		"Next Button"
	);

	NextButton->SetAnchor(E_ANCH_TOP_RIGHT);
	NextButton->SetPivot(Vector2(1.0f, 0.0f));

	NextButton->GetComponent<Transform2DComponent>()->Translate(Vector2(-20.f, 20.f));

	NextButton->SetOnClickedCallback([&]() {
		Application::Get().ThreadedCallback.Bind(this, &MirrorView::OnListView);
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

	StartOval1->GetComponent<Transform2DComponent>()->SetPosition(
		Line1->GetStartPosition(),
		false
	);

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

	// Line 2
	LineEntity2 = new StaticEntity("Line");
	LineEntity2->SetVisibility(Entity::E_COLLAPSED);

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

	StartOval2->GetComponent<Transform2DComponent>()->SetPosition(
		Line2->GetStartPosition(),
		false
	);

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

	// Mirror toggle button
	MirrorToggle = new UIButton(
		{ "0", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
		{ "0", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 1.f, 0.f) },
		{ "0", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f, 0.f, 0.f) },
		"Mirror Toggle Button"
	);
	
	MirrorToggle->SetAnchor(E_ANCH_TOP);
	MirrorToggle->SetPivot(Vector2(0.5f, 0.0f));

	MirrorToggle->GetComponent<Transform2DComponent>()->Translate(Vector2(0.f, 20.f));

	MirrorToggle->SetOnClickedCallback([&]() {
		Application::Get().ThreadedCallback.Bind(this, &MirrorView::OnMirrorToggle);
	});

	// Text
	TextEntity = new StaticEntity("Text");
	UIText* Text = TextEntity->AddComponent<UIText>("Spiegelen (tik om UI te verbergen)");
	Text->SetColor(Color(1.f));

	Text->SetAnchor(E_ANCH_BOTTOM);
	Text->SetPivot(Vector2(0.5f, 0.5f));

	Text->SetPosition(Vector2(0.f, -50.f));

	// Retrieve potential user data
	RetrieveUserData();

	// Update on first frame
	UpdateCameraUniforms();
}

void MirrorView::UpdateCameraUniforms()
{
	Window& Window = Application::Get().GetRenderContext().GetWindow();

	const float Scale = Window.GetScale();

	// Line 1
	Vector3 Start = StartOval1->GetComponent<Transform2DComponent>()->GetPosition() * Scale;
	Vector3 End = EndOval1->GetComponent<Transform2DComponent>()->GetPosition() * Scale;

	SkV2 Top = SkV2{ Start.x, Start.y };
	m_CameraImage->SetShaderUniform("top0", Top);

	SkV2 Btm = SkV2{ End.x, End.y };
	m_CameraImage->SetShaderUniform("btm0", Btm);

	// Line 2
	Vector3 Start1 = StartOval2->GetComponent<Transform2DComponent>()->GetPosition() * Scale;
	Vector3 End1 = EndOval2->GetComponent<Transform2DComponent>()->GetPosition() * Scale;

	SkV2 Top1 = SkV2{ Start1.x, Start1.y };
	m_CameraImage->SetShaderUniform("top1", Top1);

	SkV2 Btm1 = SkV2{ End1.x, End1.y };
	m_CameraImage->SetShaderUniform("btm1", Btm1);

	const float Width = m_CameraImage->GetWidth();
	const float Height = m_CameraImage->GetHeight();

	m_CameraImage->SetShaderUniform("imageSize", SkV2{ Width, Height });
	m_CameraImage->SetShaderUniform("screenSize", SkV2{ (float)Window.GetWidth(), (float)Window.GetHeight() });

	m_CameraImage->SetShaderUniform("mirrorCount", static_cast<int32_t>(MirrorCount));
}

void MirrorView::OnCameraImageData()
{
	Window& Window = Application::Get().GetRenderContext().GetWindow();
	Vector2 Dims = m_CameraImage->GetDimensions();

	const Camera& Camera = Application::Get().GetHardwareContext().GetCamera();

	if (Camera.GetOrientation() == 90 || Camera.GetOrientation() == 270)
	{
		Vector2 BakDims = Dims;

		Dims.x = Dims.y;
		Dims.y = BakDims.x;
	}

	const float Width = Window.GetWidth() / Dims.x;
	const float Height = Window.GetHeight() / Dims.y;

	m_CameraImage->SetScale(Vector2(Width, Height));
}

Vector2 MirrorView::NormToAbs(Vector2 Normalized) const
{
	AABB Bounds = m_CameraImage->GetBounds();
	
	Normalized.x *= Bounds.width();
	Normalized.y *= Bounds.height();

	Normalized.x += Bounds.fLeft;
	Normalized.y += Bounds.fTop;

	Window& Window = Application::Get().GetRenderContext().GetWindow();
	float InvScale = 1.f / Window.GetScale();

	Normalized *= InvScale;

	return Normalized;
}

Vector2 MirrorView::AbsToNorm(Vector2 Absolute) const
{
	AABB Bounds = m_CameraImage->GetBounds();

	Window& Window = Application::Get().GetRenderContext().GetWindow();
	Absolute *= Window.GetScale();

	Absolute.x -= Bounds.fLeft;
	Absolute.y -= Bounds.fTop;

	Absolute.x /= Bounds.width();
	Absolute.y /= Bounds.height();

	Absolute.x = std::clamp(Absolute.x, 0.f, 1.f);
	Absolute.y = std::clamp(Absolute.y, 0.f, 1.f);

	return Absolute;
}

void MirrorView::OnBack()
{
	SaveUserData();
	SubView::OnBack();
}

void MirrorView::OnListView()
{
	SaveUserData();

	// Switch view, discard history
	Application::Get().GetWorldManager().Clear<ListView>();
}

void MirrorView::OnMirrorToggle()
{
	MirrorCount++;
	MirrorCount %= m_ConfigFile["lineCount"].get<int>() + 1;

	MirrorToggle->SetText(std::to_string(MirrorCount));

	UpdateCameraUniforms();
}

void MirrorView::RetrieveUserData()
{
	const String Name = FileLoader::GetName(m_Path);

	String FilePath = FileLoader::GetAbsolutePath("Configs/" + Name + ".cfg", FileLoader::E_EXTERNAL);
	FILE* File = std::fopen(FilePath.c_str(), "r");

	if (!File)
		return;

	m_ConfigFile = json::from_bson(File);

	// First line
	Vector2 Start1 = NormToAbs(Vector2(
		m_ConfigFile["tar_x1_1"],
		m_ConfigFile["tar_y1_1"]
	));

	Vector2 End1 = NormToAbs(Vector2(
		m_ConfigFile["tar_x1_2"],
		m_ConfigFile["tar_y1_2"]
	));

	auto Transform = Line1->GetEntity().GetComponent<Transform2DComponent>();
	Transform->SetPosition(Start1);

	Line1->SetEndPosition(
		End1 - Start1
	);

	EndOval1->GetComponent<Transform2DComponent>()->SetPosition(
		Line1->GetEndPosition(),
		true
	);

	// Second line
	Vector2 Start2 = NormToAbs(Vector2(
		m_ConfigFile["tar_x2_1"],
		m_ConfigFile["tar_y2_1"]
	));

	Vector2 End2 = NormToAbs(Vector2(
		m_ConfigFile["tar_x2_2"],
		m_ConfigFile["tar_y2_2"]
	));

	Transform = Line2->GetEntity().GetComponent<Transform2DComponent>();
	Transform->SetPosition(Start2);

	Line2->SetEndPosition(
		End2 - Start2
	);

	EndOval2->GetComponent<Transform2DComponent>()->SetPosition(
		Line2->GetEndPosition(),
		true
	);

	// Line toggle
	LineEntity2->SetVisibility(
		m_ConfigFile["lineCount"] == 2 ? Entity::E_VISIBLE : Entity::E_COLLAPSED
	);

	// Mirror
	bool HasMirror = m_ConfigFile.contains("mirrorCount");

	if (!HasMirror)
		return;

	MirrorCount = m_ConfigFile["mirrorCount"].get<int>() % (m_ConfigFile["lineCount"].get<int>() + 1);
	MirrorToggle->SetText(std::to_string(MirrorCount));
}

void MirrorView::SaveUserData()
{
	m_ConfigFile["mirrorCount"] = MirrorCount;
	std::vector<uint8_t> BSON = nlohmann::json::to_bson(m_ConfigFile);

	// Write to config file
	const String Name = FileLoader::GetName(m_Path);
	FileLoader::Write("Configs/", Name + ".cfg", (char*)BSON.data(), BSON.size(), true, FileLoader::E_EXTERNAL);
}

void MirrorView::ToggleButtons()
{
	bool IsVisible = m_BackButton->GetVisibility() == Entity::E_VISIBLE;

	Entity::Visibility Vis = IsVisible ?
		Entity::E_COLLAPSED :
		Entity::E_VISIBLE
	;

	LineEntity1->SetVisibility(Vis);
	LineEntity2->SetVisibility(
		(!IsVisible && m_ConfigFile["lineCount"] == 2) ?
		Entity::E_VISIBLE : Entity::E_COLLAPSED
	);

	m_BackButton->SetVisibility(Vis);
	MirrorToggle->SetVisibility(Vis);
	NextButton->SetVisibility(Vis);

	TextEntity->SetVisibility(Vis);
}