#pragma once

#include "Engine/Objects/Worlds/Entities/StaticEntity.h"
#include "Platform/Hardware/Camera/Camera.h"

namespace Engine
{
	class Engine_API UICameraPreview : public StaticEntity
	{
	public:
		UICameraPreview(
			CameraType Type = E_CAMERA_BACKFACE,
			const String& sName = "Camera Preview"
		);

		~UICameraPreview();
		
	protected:
		void OnCameraStarted();

		Camera& Camera;
		
		Transform2DComponent* m_Transform2D = nullptr;
		UIImage* m_Image = nullptr;

		uint32_t m_CameraStartID = UINT_MAX;
	};
}
