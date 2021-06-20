#include "pch.h"
#include "LayeredObject.h"

namespace Engine
{
	void LayeredObject::PushLayer(Layer* Layer)
	{
		m_LayerStack.PushLayer(Layer);
	}

	void LayeredObject::PushOverlay(Layer* Layer)
	{
		m_LayerStack.PushOverlay(Layer);
	}

	void LayeredObject::Update(float fDeltaTime)
	{
		for (Layer* Layer : m_LayerStack)
		{
			if (!Layer->IsActive())
				continue;

			Layer->Update(fDeltaTime);
		}
	}

	void LayeredObject::FixedUpdate(float fDeltaTime)
	{
		for (Layer* Layer : m_LayerStack)
		{
			if (!Layer->IsActive())
				continue;

			Layer->FixedUpdate(fDeltaTime);
		}
	}

	void LayeredObject::Draw(float fDeltaTime)
	{
		for (Layer* Layer : m_LayerStack)
		{
			if (!Layer->IsActive())
				continue;

			Layer->Draw(fDeltaTime);
		}
	}

	void LayeredObject::LateUpdate(float fDeltaTime)
	{
		for (Layer* Layer : m_LayerStack)
		{
			if (!Layer->IsActive())
				continue;

			Layer->LateUpdate(fDeltaTime);
		}
	}

	bool LayeredObject::Play()
	{
		if (!DynamicObject::Play())
			return false;

		for (Layer* Layer : m_LayerStack)
		{
			Layer->Play();
		}

		return true;
	}

	bool LayeredObject::Pause()
	{
		if (!DynamicObject::Pause())
			return false;

		for (Layer* Layer : m_LayerStack)
		{
			Layer->Pause();
		}

		return true;
	}

	bool LayeredObject::Resume()
	{
		if (!DynamicObject::Resume())
			return false;

		for (Layer* Layer : m_LayerStack)
		{
			Layer->Resume();
		}

		return true;
	}

	bool LayeredObject::Stop()
	{
		if (!DynamicObject::Stop())
			return false;

		for (Layer* Layer : m_LayerStack)
		{
			Layer->Stop();
		}

		return true;
	}
}
