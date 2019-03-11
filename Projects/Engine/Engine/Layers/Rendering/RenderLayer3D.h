#pragma once

#include "Engine/Layer.h"

namespace Engine {

	class Engine_API RenderLayer3D : public Layer
	{
	public:
		RenderLayer3D();
		~RenderLayer3D();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	};

}