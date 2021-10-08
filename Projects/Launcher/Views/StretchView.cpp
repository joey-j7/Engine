#include "pch.h"
#include "StretchView.h"

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

#include "MirrorView.h"
#include "ListView.h"

using namespace Engine;

StretchView::StretchView(const String& FilePath, const String& Name) : SubView(FilePath, Name)
{
	// Camera
	/*CameraImage->SetShader(
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
	);*/

	constexpr float BtnSize = 50.f;

	// Next Button
	UIButton* NextButton = new UIButton(
		{ ">", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f) },
		{ ">", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 1.f, 0.f) },
		{ ">", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 0.f, 0.f) },
		"Next Button"
	);

	NextButton->SetAnchor(E_ANCH_TOP_RIGHT);
	NextButton->SetPivot(Vector2(1.0f, 0.0f));

	NextButton->GetComponent<Transform2DComponent>()->Translate(Vector2(-20.f, 20.f));

	NextButton->SetOnClickedCallback([&]() {
		Application::Get().ThreadedCallback.Bind(this, &StretchView::OnMirrorView);
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

	StartOval1->SetOnDraggedCallback([&](const DVector2& Delta)
	{
		SetStartPosition(Delta, *Line1, *StartOval1, *EndOval1);
	});

	EndOval1->SetOnDraggedCallback([&](const DVector2& Delta)
	{
		SetEndPosition(Delta, *Line1, *StartOval1, *EndOval1);
	});

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

	StartOval2->SetOnDraggedCallback([&](const DVector2& Delta)
	{
		SetStartPosition(Delta, *Line2, *StartOval2, *EndOval2);
	});

	EndOval2->SetOnDraggedCallback([&](const DVector2& Delta)
	{
		SetEndPosition(Delta, *Line2, *StartOval2, *EndOval2);
	});

	// Text
	StaticEntity* TextEntity = new StaticEntity("Text");
	UIText* Text = TextEntity->AddComponent<UIText>("Uitrekken");
	Text->SetColor(Color(1.f));

	Text->SetAnchor(E_ANCH_BOTTOM);
	Text->SetPivot(Vector2(0.5f, 0.5f));

	Text->SetPosition(Vector2(0.f, -50.f));

	// Retrieve potential user data
	RetrieveUserData();

	// Update on first frame
	UpdateCameraUniforms();
}

void StretchView::UpdateCameraUniforms()
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

	m_CameraImage->SetShaderUniform("screenSize", SkV2{ Width, Height });
}

void StretchView::SetStartPosition(const DVector2& Delta, UILine& Line, UIButton& StartOval, UIButton& EndOval)
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

void StretchView::SetEndPosition(const DVector2& Delta, UILine& Line, UIButton& StartOval, UIButton& EndOval)
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

void StretchView::OnCameraImageData()
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

Vector2 StretchView::NormToAbs(Vector2 Normalized) const
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

Vector2 StretchView::AbsToNorm(Vector2 Absolute) const
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

void StretchView::OnBack()
{
	SaveUserData();
	SubView::OnBack();
}

void StretchView::OnMirrorView()
{
	SaveUserData();
	new MirrorView(m_Path);
}

void StretchView::RetrieveUserData()
{
	const String Name = FileLoader::GetName(m_Path);

	String FilePath = FileLoader::GetAbsolutePath("Configs/" + Name + ".cfg", FileLoader::E_EXTERNAL);
	FILE* File = std::fopen(FilePath.c_str(), "r");

	if (!File)
		return;

	m_ConfigFile = json::from_bson(File);

	bool HasStretch = m_ConfigFile.contains("tar_x1_1");

	// First line
	Vector2 Start1 = NormToAbs(Vector2(
		HasStretch ? m_ConfigFile["tar_x1_1"] : m_ConfigFile["orig_x1_1"],
		HasStretch ? m_ConfigFile["tar_y1_1"] : m_ConfigFile["orig_y1_1"]
	));

	Vector2 End1 = NormToAbs(Vector2(
		HasStretch ? m_ConfigFile["tar_x1_2"] : m_ConfigFile["orig_x1_2"],
		HasStretch ? m_ConfigFile["tar_y1_2"] : m_ConfigFile["orig_y1_2"]
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
		HasStretch ? m_ConfigFile["tar_x2_1"] : m_ConfigFile["orig_x2_1"],
		HasStretch ? m_ConfigFile["tar_y2_1"] : m_ConfigFile["orig_y2_1"]
	));

	Vector2 End2 = NormToAbs(Vector2(
		HasStretch ? m_ConfigFile["tar_x2_2"] : m_ConfigFile["orig_x2_2"],
		HasStretch ? m_ConfigFile["tar_y2_2"] : m_ConfigFile["orig_y2_2"]
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

	fclose(File);
}

void StretchView::SaveUserData()
{
	// Line 1
	auto Transform = Line1->GetEntity().GetComponent<Transform2DComponent>();
	Vector2 Start1 = AbsToNorm(Transform->GetPosition());
	Vector2 End1 = AbsToNorm(Transform->GetPosition() + Line1->GetEndPosition());
	
	m_ConfigFile["tar_x1_1"] = Start1.x;
	m_ConfigFile["tar_y1_1"] = Start1.y;
	m_ConfigFile["tar_x1_2"] = End1.x;
	m_ConfigFile["tar_y1_2"] = End1.y;

	// Line 2
	Transform = Line2->GetEntity().GetComponent<Transform2DComponent>();
	Vector2 Start2 = AbsToNorm(Transform->GetPosition());
	Vector2 End2 = AbsToNorm(Transform->GetPosition() + Line2->GetEndPosition());

	m_ConfigFile["tar_x2_1"] = Start2.x;
	m_ConfigFile["tar_y2_1"] = Start2.y;
	m_ConfigFile["tar_x2_2"] = End2.x;
	m_ConfigFile["tar_y2_2"] = End2.y;

	std::vector<uint8_t> BSON = nlohmann::json::to_bson(m_ConfigFile);

	// Write to config file
	// const String Path = FileLoader::GetPath(m_Path);
	const String Name = FileLoader::GetName(m_Path);

	FileLoader::Write("Configs/", Name + ".cfg", (char*)BSON.data(), BSON.size(), true, FileLoader::E_EXTERNAL);
}