#include "pch.h"
#include "StretchView.h"

#include "Engine/Application.h"

#include "Engine/Objects/Components/Transform/Transform2DComponent.h"
#include "Engine/Objects/Components/Transform/Transform3DComponent.h"
#include "Engine/Objects/Components/ClickableComponent.h"
#include "Engine/Objects/Components/UI/Renderables/UIImage.h"
#include "Engine/Objects/Components/UI/Renderables/UIText.h"
#include "Engine/Objects/Components/UI/Renderables/Shapes/UILine.h"
#include "Engine/Objects/Components/UI/Renderables/Shapes/UIRect.h"
#include "Engine/Objects/Entities/UI/UIButton.h"
#include "Engine/Objects/Entities/UI/Camera/UICameraPreview.h"

#include "../PhotoEntity.h"

#include "ListView.h"

using namespace Engine;

StretchView::StretchView(const String& m_FilePath, const String& Name) : SubView(m_FilePath, Name)
{
	ClickableComponent* Clickable = m_Photo->AddComponent<ClickableComponent>();
	Clickable->OnClickedEvent.Bind(this, &StretchView::ToggleButtons);

	// Camera
	m_CameraImage->SetShader(
		"uniform shader Element;"

		"uniform float2 imageSize;"
		"uniform float2 screenSize;"

		"uniform float2 orig_top_1;"
		"uniform float2 orig_btm_1;"

		"uniform float2 orig_top_2;"
		"uniform float2 orig_btm_2;"

		"uniform float2 tar_top_1;"
		"uniform float2 tar_btm_1;"

		"uniform float2 tar_top_2;"
		"uniform float2 tar_btm_2;"

		"uniform int mirrorCount;"
		"uniform int lineCount;"

		"float2 findIntersection(float2 top, float2 btm) {"
		"  float m = (top.y - btm.y) / (top.x - btm.x);"
		"  float c = top.y - m * top.x;"
		"  return float2((sk_FragCoord.y - c) / m, sk_FragCoord.x * m + c);"
		"}"

		"half4 main(float2 Coord) {"
		"  float2 ImgCoords = Coord.xy;"

		// Find intersections
		"  float2 orig1 = findIntersection(orig_top_1, orig_btm_1);"
		"  float2 orig2 = findIntersection(orig_top_2, orig_btm_2);"

		"  float2 tar1 = findIntersection(tar_top_1, tar_btm_1);"
		"  float2 tar2 = findIntersection(tar_top_2, tar_btm_2);"
		"  float2 tarNorm1 = tar1 / screenSize;"
		"  float2 tarNorm2 = tar2 / screenSize;"

		"  if (sk_FragCoord.x < tar1.x)"
		"     ImgCoords.x = mix(0.0, orig1.x, ImgCoords.x / screenSize.x / tarNorm1.x);"
		"  else if (lineCount == 1)"
		"     ImgCoords.x = mix(orig1.x, screenSize.x, (sk_FragCoord.x - tar1.x) / (screenSize.x - tar1.x)) / screenSize.x * imageSize.x;"
		"  else {"
		"     if (sk_FragCoord.x < tar2.x)"
		"       ImgCoords.x = mix(orig1.x, orig2.x, (sk_FragCoord.x - tar1.x) / (tar2.x - tar1.x)) / screenSize.x * imageSize.x;"
		"     else"
		"       ImgCoords.x = mix(orig2.x, screenSize.x, (sk_FragCoord.x - tar2.x) / (screenSize.x - tar2.x)) / screenSize.x * imageSize.x;"
		"  }"

		// Mirror
		"  if (sk_FragCoord.x < tar1.x) {"
		"      if (mirrorCount >= 1) "
		"        ImgCoords.x = orig1.x * 2.0 / screenSize.x * imageSize.x - ImgCoords.x; }"

		"  else if (sk_FragCoord.x >= tar2.x) {"
		"      if (mirrorCount >= 2) "
		"        ImgCoords.x = orig2.x * 2.0 / screenSize.x * imageSize.x - ImgCoords.x; }"

		// Outside image
		"  if (ImgCoords.x < 0.0 || ImgCoords.x > imageSize.x ||"
		"		ImgCoords.y < 0.0 || ImgCoords.y > imageSize.y)"
		"  return half4(0.0, 0.0, 0.0, 1.0);"

		// Return result
		"  return Element.eval(ImgCoords.xy);"
		"}"
	);

	constexpr float BtnSize = 50.f;

	// Next Button
	const String Icon = "icons/icon_download.png";

	NextButton = Add<UIButton>(
		(ButtonStyle) {
		"", 5.f, Vector4(45.f), "", Icon, Color(0.f, 0.f, 0.f, 0.5f)
	},
		(ButtonStyle) {
		"", 5.f, Vector4(45.f), "", Icon, Color(1.f)
	},
		(ButtonStyle) {
		"", 5.f, Vector4(45.f), "", Icon, Color(0.882352941f, 0.2f, 0.203921569f)
	},
		"Next Button"
	);

	NextButton->GetForeground()->GetComponent<Transform2DComponent>()->SetScale(Vector2(0.25f));

	NextButton->SetAnchor(E_ANCH_TOP_RIGHT);
	NextButton->SetPivot(Vector2(1.0f, 0.0f));

	NextButton->GetComponent<Transform2DComponent>()->Translate(Vector2(-20.f, 20.f));

	NextButton->SetOnClickedCallback([&]() {
		Application::Get().ThreadedCallback.Bind(this, &StretchView::OnListView);
	});

	// Window
	Window& Window = Application::Get().GetRenderContext().GetWindow();
	float InvScale = 1.f / Window.GetScale();

	// Line 1
	LineEntity1 = Add<Entity>("Line");
	Line1 = LineEntity1->AddComponent<UILine>();

	Line1->SetStartPosition(Vector2(0.f, 0.f));
	Line1->SetEndPosition(Vector2(0.f, Window.GetHeight() * 0.8f * InvScale));
	Line1->SetColor(Color(0.882352941f, 0.2f, 0.203921569f));
	Line1->SetThickness(7);

	constexpr float OvalSize = 40.f;

	StartOval1 = Add<UIButton>(
		(ButtonStyle){ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(0.882352941f, 0.2f, 0.203921569f) },
		(ButtonStyle){ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
		(ButtonStyle){ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
		"Start Button"
	);

	StartOval1->SetParent(LineEntity1);
	StartOval1->SetAnchor(E_ANCH_TOP_LEFT);
	StartOval1->SetPivot(Vector2(0.5f, 0.5f));

	StartOval1->GetComponent<Transform2DComponent>()->SetPosition(
		Line1->GetStartPosition(),
		false
	);

	EndOval1 = Add<UIButton>(
		(ButtonStyle){ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(0.882352941f, 0.2f, 0.203921569f) },
		(ButtonStyle){ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
		(ButtonStyle){ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
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
	LineEntity2 = Add<Entity>("Line");
	LineEntity2->SetVisibility(Entity::E_COLLAPSED);

	Line2 = LineEntity2->AddComponent<UILine>();

	Line2->SetStartPosition(Vector2(0.f, 0.f));
	Line2->SetEndPosition(Vector2(0.f, Window.GetHeight() * 0.8f * InvScale));
	Line2->SetColor(Color(0.247058824f, 0.321568627f, 0.639215686f));
	Line2->SetThickness(7);

	StartOval2 = Add<UIButton>(
		(ButtonStyle){ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(0.247058824f, 0.321568627f, 0.639215686f) },
		(ButtonStyle){ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
		(ButtonStyle){ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
		"Start Button"
	);

	StartOval2->SetParent(LineEntity2);
	StartOval2->SetAnchor(E_ANCH_TOP_LEFT);
	StartOval2->SetPivot(Vector2(0.5f, 0.5f));

	StartOval2->GetComponent<Transform2DComponent>()->SetPosition(
		Line2->GetStartPosition(),
		false
	);

	EndOval2 = Add<UIButton>(
		(ButtonStyle){ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(0.247058824f, 0.321568627f, 0.639215686f) },
		(ButtonStyle){ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
		(ButtonStyle){ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f)
	},
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
	
	// Mirror toggle button
	MirrorToggle = Add<UIButton>(
		(ButtonStyle){ "0", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
		(ButtonStyle){ "0", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
		(ButtonStyle){ "0", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(0.882352941f, 0.2f, 0.203921569f) },
		"Mirror Toggle Button"
	);
	
	MirrorToggle->SetAnchor(E_ANCH_TOP);
	MirrorToggle->SetPivot(Vector2(0.5f, 0.0f));

	MirrorToggle->GetComponent<Transform2DComponent>()->Translate(Vector2(0.f, 23.f));

	MirrorToggle->SetOnClickedCallback([&]() {
		Application::Get().ThreadedCallback.Bind(this, &StretchView::OnMirrorToggle);
	});

	// Text
	TextEntity = Add<Entity>("Text");
	UIText* Text = TextEntity->AddComponent<UIText>("Uitrekken + spiegelen (tik: UI verbergen)");
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

	SkV2 Top = SkV2{ Start.y >= End.y ? Start.x : End.x, Start.y >= End.y ? Start.y : End.y };
	m_CameraImage->SetShaderUniform("tar_top_1", Top);

	SkV2 Btm = SkV2{ Start.y >= End.y ? End.x : Start.x, Start.y >= End.y ? End.y : Start.y };
	m_CameraImage->SetShaderUniform("tar_btm_1", Btm);

	// Orig
	Vector2 StartOrig = NormToAbs(Vector2(
		m_ConfigFile["orig_x1_1"],
		m_ConfigFile["orig_y1_1"]
	)) * Scale;

	Vector2 EndOrig = NormToAbs(Vector2(
		m_ConfigFile["orig_x1_2"],
		m_ConfigFile["orig_y1_2"]
	)) * Scale;

	Top = SkV2{ StartOrig.x, StartOrig.y };
	m_CameraImage->SetShaderUniform("orig_top_1", Top);

	Btm = SkV2{ EndOrig.x, EndOrig.y };
	m_CameraImage->SetShaderUniform("orig_btm_1", Btm);

	// Line 2
	Vector3 Start2 = StartOval2->GetComponent<Transform2DComponent>()->GetPosition() * Scale;
	Vector3 End2 = EndOval2->GetComponent<Transform2DComponent>()->GetPosition() * Scale;

	SkV2 Top2 = SkV2{ Start2.y >= End2.y ? Start2.x : End2.x, Start2.y >= End2.y ? Start2.y : End2.y };
	m_CameraImage->SetShaderUniform("tar_top_2", Top2);

	SkV2 Btm2 = SkV2{ Start2.y >= End2.y ? End2.x : Start2.x, Start2.y >= End2.y ? End2.y : Start2.y };
	m_CameraImage->SetShaderUniform("tar_btm_2", Btm2);

	// Orig
	StartOrig = NormToAbs(Vector2(
		m_ConfigFile["orig_x2_1"],
		m_ConfigFile["orig_y2_1"]
	)) * Scale;

	EndOrig = NormToAbs(Vector2(
		m_ConfigFile["orig_x2_2"],
		m_ConfigFile["orig_y2_2"]
	)) * Scale;


	Top = SkV2{ StartOrig.x, StartOrig.y };
	m_CameraImage->SetShaderUniform("orig_top_2", Top);

	Btm = SkV2{ EndOrig.x, EndOrig.y };
	m_CameraImage->SetShaderUniform("orig_btm_2", Btm);

	// Dimensions
	const float Width = m_CameraImage->GetWidth();
	const float Height = m_CameraImage->GetHeight();

	m_CameraImage->SetShaderUniform("imageSize", SkV2{ Width, Height });
	m_CameraImage->SetShaderUniform("screenSize", SkV2{ (float)Window.GetWidth(), (float)Window.GetHeight() });

	m_CameraImage->SetShaderUniform("mirrorCount", static_cast<int32_t>(MirrorCount));
	m_CameraImage->SetShaderUniform("lineCount", static_cast<int32_t>(LineCount));
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

void StretchView::OnMirrorToggle()
{
	MirrorCount++;
	MirrorCount %= m_ConfigFile["lineCount"].get<int>() + 1;

	MirrorToggle->SetText(std::to_string(MirrorCount));

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

void StretchView::OnListView()
{
	SaveUserData();

	// Switch view, discard history
	WorldManager::Get().ClearAndReplace<ListView>();
}

void StretchView::RetrieveUserData()
{
	const String Name = FileLoader::GetName(m_Path);

	String m_FilePath = FileLoader::GetAbsolutePath("Configs/" + Name + ".cfg", FileLoader::E_EXTERNAL);
	FILE* File = std::fopen(m_FilePath.c_str(), "r");

	if (!File)
		return;

	m_ConfigFile = json::from_bson(File);
	fclose(File);

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

	// Line count
	LineCount = m_ConfigFile["lineCount"].get<int>();

	// Mirror
	bool HasMirror = m_ConfigFile.contains("mirrorCount");

	if (!HasMirror)
		return;

	MirrorCount = m_ConfigFile["mirrorCount"].get<int>() % (m_ConfigFile["lineCount"].get<int>() + 1);
	MirrorToggle->SetText(std::to_string(MirrorCount));
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

	m_ConfigFile["mirrorCount"] = MirrorCount;

	std::vector<uint8_t> BSON = nlohmann::json::to_bson(m_ConfigFile);

	// Write to config file
	// const String Path = FileLoader::GetPath(m_Path);
	const String Name = FileLoader::GetName(m_Path);

	FileLoader::Write("Configs/", Name + ".cfg", (char*)BSON.data(), BSON.size(), true, FileLoader::E_EXTERNAL);
}

void StretchView::ToggleButtons()
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