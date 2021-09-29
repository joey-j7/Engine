#include "pch.h"
#include "UICameraPreview.h"

#include "Engine/Application.h"

#include "Engine/Objects/Worlds/Entities/Components/Transform/Transform2DComponent.h"
#include "Engine/Objects/Worlds/Entities/Components/UI/Renderables/UIImage.h"

namespace Engine
{
	UICameraPreview::UICameraPreview(CameraType Type, const String& sName) : Camera(Application::Get().GetHardwareContext().GetCamera())
	{
		m_Transform2D = AddComponent<Transform2DComponent>();
		m_Image = AddComponent<UIImage>();

		m_Image->ScaleWithDPI(false);
		m_Image->SetPivot(Vector2(0.5f));
		m_Image->SetAnchor(E_ANCH_CENTER);

		m_CameraStartID = Camera.OnStartCallback.Bind(this, &UICameraPreview::OnCameraStarted);

		Camera.Start();
	}

	UICameraPreview::~UICameraPreview()
	{
		Camera.OnStartCallback.Unbind(m_CameraStartID);
		Camera.Stop();

		Camera.SetPreviewImage(nullptr);

		m_Image = nullptr;
	}

	void UICameraPreview::OnCameraStarted()
	{
		m_Transform2D->SetRotation(
			static_cast<float>(Camera.GetOrientation())
		);

		Window& Window = Application::Get().GetRenderContext().GetWindow();
		
		// m_Image->SetWidth(Camera.GetOrientation() == 90 || Camera.GetOrientation() == 270 ? Height : Width);
		// m_Image->SetHeight(Camera.GetOrientation() == 90 || Camera.GetOrientation() == 270 ? Width : Height);

		Camera.SetPreviewImage(m_Image);
		Camera.OnStartCallback.Unbind(m_CameraStartID);
	}
}
