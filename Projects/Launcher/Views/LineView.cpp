#include "pch.h"
#include "LineView.h"

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

#include "CameraView.h"
#include "StretchView.h"

using namespace Engine;

LineView::LineView(const String& m_FilePath, const String& Name) : SubView(m_FilePath, Name)
{
	constexpr float BtnSize = 50.f;

	// Next Button
	String Icon = "icons/icon_next_w.png";

	UIButton* NextButton = Add<UIButton>(
		(ButtonStyle) {
		"", 5.f, Vector4(45.f), "", Icon, Color(0.f, 0.f, 0.f, 0.5f)
	},
		(ButtonStyle) {
		"", 5.f, Vector4(45.f), "", Icon, Color(0.f, 0.f, 0.f, 0.5f)
	},
		(ButtonStyle) {
		"", 5.f, Vector4(45.f), "", Icon, Color(0.882352941f, 0.2f, 0.203921569f)
	},
		"Next Button"
	);

	NextButton->GetForeground()->GetComponent<Transform2DComponent>()->SetScale(Vector2(0.75f));

	NextButton->SetAnchor(E_ANCH_TOP_RIGHT);
	NextButton->SetPivot(Vector2(1.0f, 0.0f));

	NextButton->GetComponent<Transform2DComponent>()->Translate(Vector2(-20.f, 20.f));

	NextButton->SetOnClickedCallback([&]() {
		Application::Get().ThreadedCallback.Bind(this, &LineView::OnStretchView);
	});

	// Window
	Window& Window = Application::Get().GetRenderContext().GetWindow();
	float InvScale = 1.f / Window.GetScale();

	// Line 1
	LineEntity1 = Add<Entity>("Line");
	Line1 = LineEntity1->AddComponent<UILine>();

	Line1->SetStartPosition(Vector2(0.f, 0.f));
	Line1->SetEndPosition(Vector2(0.1f, Window.GetHeight() * 0.8f * InvScale));
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
	Line2->SetEndPosition(Vector2(0.1f, Window.GetHeight() * 0.8f * InvScale));
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
		(ButtonStyle){ "", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
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

	// Line toggle button
	LineToggle = Add<UIButton>(
		(ButtonStyle){ "2", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
		(ButtonStyle){ "2", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(1.f) },
		(ButtonStyle){ "2", OvalSize * 2.f, Vector2(OvalSize), "", "", Color(0.882352941f, 0.2f, 0.203921569f) },
		"Line Toggle Button"
	);
	
	LineToggle->SetAnchor(E_ANCH_TOP);
	LineToggle->SetPivot(Vector2(0.5f, 0.0f));

	LineToggle->GetComponent<Transform2DComponent>()->Translate(Vector2(0.f, 23.f));

	LineToggle->SetOnClickedCallback([&]() {
		Application::Get().ThreadedCallback.Bind(this, &LineView::OnLineToggle);
	});

	// Text
	Entity* TextEntity = Add<Entity>("Text");
	UIText* Text = TextEntity->AddComponent<UIText>("Sleep de lijn(en) over de naden");
	Text->SetColor(Color(1.f));

	Text->SetAnchor(E_ANCH_BOTTOM);
	Text->SetPivot(Vector2(0.5f, 0.5f));

	Text->SetPosition(Vector2(0.f, -50.f));

	// Retrieve potential user data
	RetrieveUserData();

	// Check if we're coming from camera view
	if (WorldManager::Get().Has<CameraView>())
	{
		m_BackButton->SetOnClickedCallback([&]() {
			Application::Get().ThreadedCallback.Bind(this, &LineView::OnCameraView);
		});

		Icon = "icons/icon_trash.png";

		ButtonStyle Default = m_BackButton->GetDefaultStyle();
		Default.m_ForegroundImagePath = Icon;
		m_BackButton->SetDefaultStyle(Default);

		ButtonStyle Hover = m_BackButton->GetHoverStyle();
		Hover.m_ForegroundImagePath = Icon;
		m_BackButton->SetHoverStyle(Hover);

		ButtonStyle Press = m_BackButton->GetPressStyle();
		Press.m_ForegroundImagePath = Icon;
		m_BackButton->SetPressStyle(Press);

		m_BackButton->GetForeground()->GetComponent<Transform2DComponent>()->SetScale(Vector2(0.25f));
	}
}

void LineView::SetStartPosition(const DVector2& Delta, UILine& Line, UIButton& StartOval, UIButton& EndOval)
{
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
}

void LineView::SetEndPosition(const DVector2& Delta, UILine& Line, UIButton& StartOval, UIButton& EndOval)
{
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
}

void LineView::OnCameraImageData()
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

Vector2 LineView::NormToAbs(Vector2 Normalized) const
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

Vector2 LineView::AbsToNorm(Vector2 Absolute) const
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

void LineView::OnLineToggle()
{
	m_LineToggled = !m_LineToggled;
	LineToggle->SetText(m_LineToggled ? "1" : "2");
	
	LineEntity2->SetVisibility(
		m_LineToggled ? Entity::E_VISIBLE : Entity::E_COLLAPSED
	);
}

void LineView::OnCameraView()
{
	const String Path = FileLoader::GetPath(m_Path);
	const String Name = FileLoader::GetName(m_Path);
	const String Extension = FileLoader::GetExtension(m_Path);

	FileLoader::Delete(Path, Name + "." + Extension, FileLoader::Type::E_ROOT);
	FileLoader::Delete("Configs/", Name + ".cfg", FileLoader::Type::E_EXTERNAL);

	delete this;
}

void LineView::OnStretchView()
{
	SaveUserData();
	WorldManager::Get().Push<StretchView>(m_Path);
}

void LineView::RetrieveUserData()
{
	const String Name = FileLoader::GetName(m_Path);

	String m_FilePath = FileLoader::GetAbsolutePath("Configs/" + Name + ".cfg", FileLoader::E_EXTERNAL);
	FILE* File = std::fopen(m_FilePath.c_str(), "r");

	if (!File)
		return;

	m_ConfigFile = json::from_bson(File);
	m_LineToggled = m_ConfigFile["lineCount"] == 2;

	// First line
	Vector2 Start1 = NormToAbs(Vector2(m_ConfigFile["orig_x1_1"], m_ConfigFile["orig_y1_1"]));
	Vector2 End1 = NormToAbs(Vector2(m_ConfigFile["orig_x1_2"], m_ConfigFile["orig_y1_2"]));

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
	Vector2 Start2 = NormToAbs(Vector2(m_ConfigFile["orig_x2_1"], m_ConfigFile["orig_y2_1"]));
	Vector2 End2 = NormToAbs(Vector2(m_ConfigFile["orig_x2_2"], m_ConfigFile["orig_y2_2"]));
	
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
	LineToggle->SetText(m_LineToggled ? "1" : "2");
	LineEntity2->SetVisibility(
		m_LineToggled ? Entity::E_VISIBLE : Entity::E_COLLAPSED
	);

	fclose(File);
}

void LineView::SaveUserData()
{
	// Create binary config
	m_ConfigFile["lineCount"] = m_LineToggled ? 2 : 1;

	// Line 1
	auto Transform = Line1->GetEntity().GetComponent<Transform2DComponent>();
	Vector2 Start1 = AbsToNorm(Transform->GetPosition());
	Vector2 End1 = AbsToNorm(Transform->GetPosition() + Line1->GetEndPosition());
	
	m_ConfigFile["orig_x1_1"] = Start1.x;
	m_ConfigFile["orig_y1_1"] = Start1.y;
	m_ConfigFile["orig_x1_2"] = End1.x;
	m_ConfigFile["orig_y1_2"] = End1.y;

	// Line 2
	Transform = Line2->GetEntity().GetComponent<Transform2DComponent>();
	Vector2 Start2 = AbsToNorm(Transform->GetPosition());
	Vector2 End2 = AbsToNorm(Transform->GetPosition() + Line2->GetEndPosition());

	m_ConfigFile["orig_x2_1"] = Start2.x;
	m_ConfigFile["orig_y2_1"] = Start2.y;
	m_ConfigFile["orig_x2_2"] = End2.x;
	m_ConfigFile["orig_y2_2"] = End2.y;

	std::vector<uint8_t> BSON = nlohmann::json::to_bson(m_ConfigFile);

	// Write to config file
	// const String Path = FileLoader::GetPath(m_Path);
	const String Name = FileLoader::GetName(m_Path);

	FileLoader::Write("Configs/", Name + ".cfg", (char*)BSON.data(), BSON.size(), true, FileLoader::E_EXTERNAL);
}