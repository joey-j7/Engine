#include "pch.h"
#include "PhotoView.h"

#include "LineView.h"
#include "ListView.h"

#include "Engine/Objects/Worlds/Entities/UI/UIButton.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Renderables/UIImage.h"

#include "../PhotoEntity.h"

using namespace Engine;

PhotoView::PhotoView(const String& FilePath, const String& Name) : SubView(FilePath, Name)
{
	ClickableComponent* Clickable = m_Photo->AddComponent<ClickableComponent>();
	Clickable->OnClickedEvent.Bind(this, &PhotoView::ToggleButtons);

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

	// Edit button
	m_EditButton = new UIButton(
		{ "*", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f) },
		{ "*", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 1.f, 0.f) },
		{ "*", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 0.f, 0.f) },
		"Edit Button"
	);

	m_EditButton->SetAnchor(E_ANCH_BOTTOM_LEFT);
	m_EditButton->SetPivot(Vector2(0.0f, 1.0f));

	m_EditButton->GetComponent<Transform2DComponent>()->Translate(Vector2(20.f, -20.f));

	m_EditButton->SetOnClickedCallback([&]() {
		Application::Get().ThreadedCallback.Bind(this, &PhotoView::OnLineView);
	});

	// Delete button
	m_DeleteButton = new UIButton(
		{ "x", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f) },
		{ "x", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 1.f, 0.f) },
		{ "x", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 0.f, 0.f) },
		"Delete Button"
	);

	m_DeleteButton->SetAnchor(E_ANCH_TOP_RIGHT);
	m_DeleteButton->SetPivot(Vector2(1.0f, 0.0f));

	m_DeleteButton->GetComponent<Transform2DComponent>()->Translate(Vector2(-20.f, 20.f));

	m_DeleteButton->SetOnClickedCallback([&]() {
		Application::Get().ThreadedCallback.Bind(this, &PhotoView::OnDelete);
	});

	RetrieveUserData();
	UpdateCameraUniforms();
}

PhotoView::~PhotoView()
{

}

void PhotoView::RetrieveUserData()
{
	const String Name = FileLoader::GetName(m_Path);

	String FilePath = FileLoader::GetAbsolutePath("Configs/" + Name + ".cfg", FileLoader::E_EXTERNAL);
	FILE* File = std::fopen(FilePath.c_str(), "r");

	if (!File)
		return;

	m_ConfigFile = json::from_bson(File);
	fclose(File);

	// Line count
	LineCount = m_ConfigFile["lineCount"].get<int>();

	// Mirror
	bool HasMirror = m_ConfigFile.contains("mirrorCount");

	if (!HasMirror)
		return;

	MirrorCount = m_ConfigFile["mirrorCount"].get<int>() % (m_ConfigFile["lineCount"].get<int>() + 1);
}

Engine::Vector2 PhotoView::NormToAbs(Engine::Vector2 Normalized) const
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

void PhotoView::UpdateCameraUniforms()
{
	Window& Window = Application::Get().GetRenderContext().GetWindow();

	const float Scale = Window.GetScale();

	bool HasStretch = m_ConfigFile.contains("tar_x1_1");

	// First line
	Vector2 StartOrig1 = Vector2(0.f);
	Vector2 EndOrig1 = Vector2(0.01f, 1.f);

	if (m_ConfigFile.contains("orig_x1_1"))
	{
		StartOrig1.x = m_ConfigFile["orig_x1_1"].get<float>();
		StartOrig1.y = m_ConfigFile["orig_y1_1"].get<float>();

		EndOrig1.x = m_ConfigFile["orig_x1_2"].get<float>();
		EndOrig1.y = m_ConfigFile["orig_y1_2"].get<float>();
	}

	Vector2 Start = NormToAbs(Vector2(
		HasStretch ? m_ConfigFile["tar_x1_1"].get<float>() : StartOrig1.x,
		HasStretch ? m_ConfigFile["tar_y1_1"].get<float>() : StartOrig1.y
	)) * Scale;

	Vector2 End = NormToAbs(Vector2(
		HasStretch ? m_ConfigFile["tar_x1_2"].get<float>() : EndOrig1.x,
		HasStretch ? m_ConfigFile["tar_y1_2"].get<float>() : EndOrig1.y
	)) * Scale;

	SkV2 Top = SkV2{ Start.y >= End.y ? Start.x : End.x, Start.y >= End.y ? Start.y : End.y };
	m_CameraImage->SetShaderUniform("tar_top_1", Top);

	SkV2 Btm = SkV2{ Start.y >= End.y ? End.x : Start.x, Start.y >= End.y ? End.y : Start.y };
	m_CameraImage->SetShaderUniform("tar_btm_1", Btm);

	// Orig
	Vector2 StartOrig = NormToAbs(Vector2(
		StartOrig1.x,
		StartOrig1.y
	)) * Scale;

	Vector2 EndOrig = NormToAbs(Vector2(
		EndOrig1.x,
		EndOrig1.y
	)) * Scale;

	Top = SkV2{ StartOrig.x, StartOrig.y };
	m_CameraImage->SetShaderUniform("orig_top_1", Top);

	Btm = SkV2{ EndOrig.x, EndOrig.y };
	m_CameraImage->SetShaderUniform("orig_btm_1", Btm);

	// Line 2
	Vector2 StartOrig2 = Vector2(1.f, 0.f);
	Vector2 EndOrig2 = Vector2(0.99f, 1.f);

	if (m_ConfigFile.contains("orig_x2_1"))
	{
		StartOrig2.x = m_ConfigFile["orig_x2_1"].get<float>();
		StartOrig2.y = m_ConfigFile["orig_y2_1"].get<float>();

		EndOrig2.x = m_ConfigFile["orig_x2_2"].get<float>();
		EndOrig2.y = m_ConfigFile["orig_y2_2"].get<float>();
	}

	Vector2 Start2 = NormToAbs(Vector2(
		HasStretch ? m_ConfigFile["tar_x2_1"].get<float>() : StartOrig2.x,
		HasStretch ? m_ConfigFile["tar_y2_1"].get<float>() : StartOrig2.y
	)) * Scale;

	Vector2 End2 = NormToAbs(Vector2(
		HasStretch ? m_ConfigFile["tar_x2_2"].get<float>() : EndOrig2.x,
		HasStretch ? m_ConfigFile["tar_y2_2"].get<float>() : EndOrig2.y
	)) * Scale;

	SkV2 Top2 = SkV2{ Start2.y >= End2.y ? Start2.x : End2.x, Start2.y >= End2.y ? Start2.y : End2.y };
	m_CameraImage->SetShaderUniform("tar_top_2", Top2);

	SkV2 Btm2 = SkV2{ Start2.y >= End2.y ? End2.x : Start2.x, Start2.y >= End2.y ? End2.y : Start2.y };
	m_CameraImage->SetShaderUniform("tar_btm_2", Btm2);

	// Orig
	StartOrig = NormToAbs(Vector2(
		StartOrig2.x,
		StartOrig2.y
	)) * Scale;

	EndOrig = NormToAbs(Vector2(
		EndOrig2.x,
		EndOrig2.y
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

void PhotoView::OnLineView()
{
	new LineView(m_Path);
}

void PhotoView::OnDelete()
{
	const String Path = FileLoader::GetPath(m_Path);
	const String Name = FileLoader::GetName(m_Path);
	const String Extension = FileLoader::GetExtension(m_Path);

	FileLoader::Delete(Path, Name + "." + Extension, FileLoader::Type::E_ROOT);
	FileLoader::Delete("Configs/", Name + ".cfg", FileLoader::Type::E_EXTERNAL);

	// Switch view, discard history
	Application::Get().GetWorldManager().Clear<ListView>();
}

void PhotoView::ToggleButtons()
{
	Entity::Visibility Vis = m_BackButton->GetVisibility() == Entity::E_VISIBLE ?
		Entity::E_COLLAPSED :
		Entity::E_VISIBLE
	;

	m_BackButton->SetVisibility(Vis);
	m_EditButton->SetVisibility(Vis);
	m_DeleteButton->SetVisibility(Vis);
}
