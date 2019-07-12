#pragma once

namespace Engine
{
	class Engine_API RenderLayer3D : public Layer
	{
	public:
		RenderLayer3D();
		~RenderLayer3D();

	private:
		virtual void OnAttach(const LayerStack& stack) override;
		virtual bool OnDetach(const LayerStack& stack) override;

		virtual void Draw(float fDeltaTime) override;
	};
}