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

CameraView::CameraView(const String& Name) : SubView(Name)
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
	if (Application::Get().GetWorldManager().GetActive() == this)
		new LineView(Path);
}

void CameraView::OnPhotoTaken(const String& FilePath)
{
	OnLineView(FilePath);
}