#include "pch.h"

#include "CameraView.h"
#include "LineView.h"
#include "ListView.h"

#include "Engine/Application.h"

#include "Engine/Objects/Worlds/Entities/UI/Camera/UICameraPreview.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Renderables/UIImage.h"
#include "Engine/Objects/Worlds/Entities/UI/UIButton.h"

#include "Engine/Objects/Worlds/Entities/Components/UI/Renderables/Shapes/UILine.h"

using namespace Engine;

CameraView::CameraView()
{
	UICameraPreview* CameraPreview = new UICameraPreview();
	CameraImage = CameraPreview->GetComponent<UIImage>();

	uint32_t m_ImageDataEventID = CameraImage->OnImageDataReceived.Bind(this, &CameraView::OnCameraImageData);
	Application::Get().GetHardwareContext().GetCamera().OnPhotoTakenCallback.Bind(this, &CameraView::OnPhotoTaken);

	float BtnSize = 80.f;

	UIButton* Button = new UIButton(
		{ "Foto", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f) },
		{ "Foto", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 1.f, 0.f) },
		{ "Foto", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 0.f, 0.f) },
		"Camera Button"
	);

	Button->SetAnchor(E_ANCH_BOTTOM);
	Button->SetPivot(Vector2(0.5f, 0.5f));

	Button->GetComponent<Transform2DComponent>()->Translate(Vector2(0.f, -80.f));

	Button->SetOnClickedCallback([]() {
		Application::Get().GetHardwareContext().GetCamera().TakePhoto();
	});

	BtnSize = 50.f;

	UIButton* BackButton = new UIButton(
		{ "<", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f) },
		{ "<", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 1.f, 0.f) },
		{ "<", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 0.f, 0.f) },
		"Back Button"
	);

	BackButton->SetAnchor(E_ANCH_TOP_LEFT);
	BackButton->SetPivot(Vector2(0.0f, 0.0f));

	BackButton->GetComponent<Transform2DComponent>()->Translate(Vector2(20.f, 20.f));

	BackButton->SetOnClickedCallback([&]() {
		Application::Get().ThreadedCallback.Bind(this, &CameraView::OnListView);
	});
}

CameraView::~CameraView()
{
	CameraImage->OnImageDataReceived.Unbind(m_ImageDataEventID);
}

void CameraView::OnCameraImageData()
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

	const float Scale = glm::min(Width, Height);

	CameraImage->SetScale(Vector2(Scale, Scale));
}

void CameraView::OnLineView(const String& Path)
{
	Application::Get().GetWorldManager().Remove(
		Application::Get().GetWorldManager().GetActive()
	);

	LineView* View = new LineView(Path);
}

void CameraView::OnListView()
{
	Application::Get().GetWorldManager().Remove(
		Application::Get().GetWorldManager().GetActive()
	);

	ListView* View = new ListView();
}

void CameraView::OnPhotoTaken(const String& FilePath)
{
	OnLineView(FilePath);
}

void CameraView::SetStartPosition(const Engine::DVector2& Delta, Engine::UILine& Line, Engine::UIButton& EndOval)
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

void CameraView::SetEndPosition(const DVector2& Delta, Engine::UILine& Line, Engine::UIButton& EndOval)
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

void CameraView::UpdateCameraUniforms()
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
