#pragma once

#include "Engine/Layers/Layer.h"

namespace Engine
{
	class Engine_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void Begin();
		void End();

		virtual void OnAttach(const LayerStack& stack) override;
		virtual bool OnDetach(const LayerStack& stack) override;

	private:
		virtual void Draw(float fDeltaTime) override;
	};
}