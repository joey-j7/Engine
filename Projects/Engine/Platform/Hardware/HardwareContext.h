#pragma once

#include "Camera/Camera.h"

namespace Engine
{
	class Engine_API HardwareContext
	{
	public:
		Camera& GetCamera();

		void Pause();
		void Resume();

	private:
		std::unique_ptr<Camera> m_pCamera;
	};
}
