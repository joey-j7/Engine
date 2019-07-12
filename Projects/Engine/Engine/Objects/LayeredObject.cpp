#include "pch.h"
#include "LayeredObject.h"

namespace Engine
{
	void LayeredObject::PushLayer(Layer* pLayer)
	{
		m_LayerStack.PushLayer(pLayer);
	}

	void LayeredObject::PushOverlay(Layer* pLayer)
	{
		m_LayerStack.PushOverlay(pLayer);
	}

	void LayeredObject::Update(float fDeltaTime)
	{
		for (Layer* pLayer : m_LayerStack)
		{
			if (!pLayer->IsActive())
				continue;

			pLayer->Update(fDeltaTime);
		}
	}

	void LayeredObject::FixedUpdate(float fDeltaTime)
	{
		for (Layer* pLayer : m_LayerStack)
		{
			if (!pLayer->IsActive())
				continue;

			pLayer->FixedUpdate(fDeltaTime);
		}
	}

	void LayeredObject::Draw(float fDeltaTime)
	{
		for (Layer* pLayer : m_LayerStack)
		{
			if (!pLayer->IsActive())
				continue;

			pLayer->Draw(fDeltaTime);
		}
	}

	void LayeredObject::LateUpdate(float fDeltaTime)
	{
		for (Layer* pLayer : m_LayerStack)
		{
			if (!pLayer->IsActive())
				continue;

			pLayer->LateUpdate(fDeltaTime);
		}
	}

	bool LayeredObject::Play()
	{
		if (!RealTimeObject::Play())
			return false;

		for (Layer* pLayer : m_LayerStack)
		{
			pLayer->Play();
		}

		return true;
	}

	bool LayeredObject::Pause()
	{
		if (!RealTimeObject::Pause())
			return false;

		for (Layer* pLayer : m_LayerStack)
		{
			pLayer->Pause();
		}

		return true;
	}

	bool LayeredObject::Resume()
	{
		if (!RealTimeObject::Resume())
			return false;

		for (Layer* pLayer : m_LayerStack)
		{
			pLayer->Resume();
		}

		return true;
	}

	bool LayeredObject::Stop()
	{
		if (!RealTimeObject::Stop())
			return false;

		for (Layer* pLayer : m_LayerStack)
		{
			pLayer->Stop();
		}

		return true;
	}

	void LayeredObject::Call(Event& e)
	{
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
		{
			(*--it)->Call(e);
			if (e.Handled)
				break;
		}
	}
}
