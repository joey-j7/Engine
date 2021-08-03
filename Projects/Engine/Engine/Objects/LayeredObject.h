#pragma once

#include "Object.h"

#include "Engine/Layers/LayerStack.h"

namespace Engine
{
	class Engine_API LayeredObject : public DynamicObject
	{
	public:
		LayeredObject(const String& sName = "Layered Object") : DynamicObject(sName) {};

		void PushLayer(Layer* pLayer);
		void PushOverlay(Layer* pLayer);

	protected:
		virtual void Update(float fDeltaTime) override;
		virtual void FixedUpdate(float fDeltaTime) override;

		virtual void Draw(float fDeltaTime) override;

		virtual void LateUpdate(float fDeltaTime) override;

		virtual bool Play() override;
		virtual bool Pause() override;
		virtual bool Resume() override;
		virtual bool Stop() override;

		LayerStack m_LayerStack;
	};
}
