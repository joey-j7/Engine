#pragma once

#include "Engine/Core.h"
#include "Engine/Objects/DynamicObject.h"

namespace Engine
{
	class LayerStack;

	class Engine_API Layer : public DynamicObject
	{
		friend class LayeredObject;

	public:
		Layer(const std::string& name = "Unnamed Layer");
		virtual ~Layer();

		void Reset(const LayerStack& stack)
		{
			OnDetach(stack);
			OnAttach(stack);
		};

		virtual void OnAttach(const LayerStack& stack);
		virtual bool OnDetach(const LayerStack& stack);

	private:
		virtual void Update(float fDeltaTime) override {};
		virtual void FixedUpdate(float fDeltaTime) override {};

		virtual void LateUpdate(float fDeltaTime) override {};
		virtual void Draw(float fDeltaTime) override {};

		uint32_t m_uiAttachCount = 0;
	};

}
