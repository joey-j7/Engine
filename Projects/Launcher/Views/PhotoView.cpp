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

	// Line 1
	Vector2 Start = NormToAbs(Vector2(
		m_ConfigFile["tar_x1_1"],
		m_ConfigFile["tar_y1_1"]
	)) * Scale;

	Vector2 End = NormToAbs(Vector2(
		m_ConfigFile["tar_x1_2"],
		m_ConfigFile["tar_y1_2"]
	)) * Scale;

	SkV2 Top = SkV2{ Start.x, Start.y };
	m_CameraImage->SetShaderUniform("top0", Top);

	SkV2 Btm = SkV2{ End.x, End.y };
	m_CameraImage->SetShaderUniform("btm0", Btm);

	// Line 2
	Vector2 Start1 = NormToAbs(Vector2(
		m_ConfigFile["tar_x2_1"],
		m_ConfigFile["tar_y2_1"]
	)) * Scale;

	Vector2 End1 = NormToAbs(Vector2(
		m_ConfigFile["tar_x2_2"],
		m_ConfigFile["tar_y2_2"]
	)) * Scale;

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
