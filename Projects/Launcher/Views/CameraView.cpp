#include "pch.h"

#include "CameraView.h"
#include "LineView.h"
#include "ListView.h"

#include "Engine/Application.h"

#include "Engine/Objects/Entities/UI/Camera/UICameraPreview.h"
#include "Engine/Objects/Components/UI/Renderables/UIImage.h"
#include "Engine/Objects/Entities/UI/UIButton.h"

#include "Engine/Objects/Components/UI/Renderables/Shapes/UILine.h"

using namespace Engine;

CameraView::CameraView(const String& Name)
{
	UICameraPreview* CameraPreview = Add<UICameraPreview>();
	CameraImage = CameraPreview->GetComponent<UIImage>();

	uint32_t m_ImageDataEventID = CameraImage->OnImageDataReceived.Bind(this, &CameraView::OnCameraImageData);
	Application::Get().GetHardwareContext().GetCamera().OnPhotoTakenCallback.Bind(this, &CameraView::OnPhotoTaken);

	float BtnSize = 90.f;

	// Camera button
	UIButton* Button = Add<UIButton>(
		(ButtonStyle){ "", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f),
		UIRenderable::Gradient(), 5, Color(1.f), false
	},
		(ButtonStyle){ "", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(1.f, 1.f, 0.f, 0.5f),
		UIRenderable::Gradient(), 5, Color(1.f), false
	},
		(ButtonStyle){ "", BtnSize * 2.f, Vector4(BtnSize), "", "", Color(0.882352941f, 0.2f, 0.203921569f),
		UIRenderable::Gradient(), 5, Color(0.882352941f, 0.2f, 0.203921569f), false
	},
		"Camera Button"
	);

	Button->SetAnchor(E_ANCH_BOTTOM);
	Button->SetPivot(Vector2(0.5f, 0.5f));

	Button->GetComponent<Transform2DComponent>()->Translate(Vector2(0.f, -80.f));

	Button->SetOnClickedCallback([]() {
		Application::Get().GetHardwareContext().GetCamera().TakePhoto();
	});

	// List view button
	BtnSize = 70.f;

	Button = Add<UIButton>(
		(ButtonStyle){ "", BtnSize * 2.f, Vector4(BtnSize), "", "icons/icon_gallery.png", Color(0.f, 0.f, 0.f, 0.5f)
	},
		(ButtonStyle){ "", BtnSize * 2.f, Vector4(BtnSize), "", "icons/icon_gallery.png", Color(1.f, 1.f, 0.f, 0.5f)
	},
		(ButtonStyle){ "", BtnSize * 2.f, Vector4(BtnSize), "", "icons/icon_gallery.png", Color(0.882352941f, 0.2f, 0.203921569f, 0.5f)
	},
		"List View Button"
	);

	Button->SetAnchor(E_ANCH_BOTTOM);
	Button->SetPivot(Vector2(0.5f, 0.5f));

	Button->GetForeground()->GetComponent<Transform2DComponent>()->SetScale(0.25f);

	Button->GetComponent<Transform2DComponent>()->Translate(Vector2(-110.f, -80.f));

	Button->SetOnClickedCallback([this]() {
		Application::Get().ThreadedCallback.Bind(this, &CameraView::OnListView);
	});

	// Ask for read/write storage permissions
	bool Permission = PermissionManager::Get().HasPermission(PermissionManager::E_READ_EXTERNAL_STORAGE);

	if (Permission)
		Permission = PermissionManager::Get().HasPermission(PermissionManager::E_WRITE_EXTERNAL_STORAGE);

	if (!Permission)
	{
		PermissionManager::Get().RequestPermissions(
			{ PermissionManager::E_READ_EXTERNAL_STORAGE, PermissionManager::E_WRITE_EXTERNAL_STORAGE }
		);
	}

	// Trash view button
	/*Button = Add<UIButton>(
		(ButtonStyle){ "", BtnSize * 2.f, Vector4(BtnSize), "", "icons/icon_trash.png", Color(0.f, 0.f, 0.f, 0.5f)
	},
		(ButtonStyle){ "", BtnSize * 2.f, Vector4(BtnSize), "", "icons/icon_trash.png", Color(1.f, 1.f, 0.f, 0.5f)
	},
		(ButtonStyle){ "", BtnSize * 2.f, Vector4(BtnSize), "", "icons/icon_trash.png", Color(0.882352941f, 0.2f, 0.203921569f, 0.5f)
	},
		"Trash View Button"
	);

	Button->GetForeground()->GetComponent<Transform2DComponent>()->SetScale(0.25f);

	Button->SetAnchor(E_ANCH_BOTTOM);
	Button->SetPivot(Vector2(0.5f, 0.5f));

	Button->GetComponent<Transform2DComponent>()->Translate(Vector2(110.f, -80.f));

	Button->SetOnClickedCallback([this]() {
		Application::Get().ThreadedCallback.Bind(this, &CameraView::OnTrashView);
	});*/
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
	if (WorldManager::Get().GetLayer() == this)
		WorldManager::Get().Push<LineView>(Path);
}

void CameraView::OnListView()
{
	// Switch view, discard history
	WorldManager::Get().ClearAndReplace<ListView>();
}

void CameraView::OnTrashView()
{
	// Switch view, discard history
	WorldManager::Get().ClearAndReplace<ListView>();
}

void CameraView::OnPhotoTaken(const String& m_FilePath)
{
	OnLineView(m_FilePath);
}