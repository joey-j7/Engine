#pragma once

#include "Engine/Objects/Object.h"
#include "Engine/Objects/Stateable.h"

#include "Layer.h"

// A container of layers and overlays, which can be activated/deactivated in the form of
// a stateable. The left side of the stack has layers, the right side overlays
// Looping through the active layers should go from index m_LayerIndex till .end()

namespace Engine
{
	template <class L = Layer>
	class Engine_API Layerable
	{
		static_assert(
			std::is_base_of<Layer, L>::value,
			"Type is not a Layer"
		);

	public:
		Layerable() {};
		~Layerable()
		{
			Clear();
		}

		template <class T, class... Args>
		T* Push(Args... Arguments)
		{
			static_assert(
				std::is_base_of<L, T>::value,
				"Type is not a correct Layer"
			);

			// Pause previous layer
			if (L* Old = GetLayer())
			{
				Old->Pause();
			}

			T* Layer = new T(Arguments...);

			m_Stack.emplace(m_Stack.begin() + m_LayerIndex, Layer);
			m_LayerIndex++;
			
			Layer->OnAttach();
			return Layer;
		}

		template <class T, class... Args>
		T* PushOverlay(Args... Arguments)
		{
			static_assert(
				std::is_base_of<L, T>::value,
				"Type is not a correct Layer"
			);

			T* Layer = new T(Arguments...);
			m_Stack.emplace_back(Layer);

			Layer->OnAttach();
			return Layer;
		}

		virtual bool Pop(L& Layer, bool Delete = true)
		{
			auto it = std::find(m_Stack.begin(), m_Stack.end(), &Layer);

			if (it == m_Stack.end())
				return false;

			bool IsOverlay = it > m_Stack.begin() + m_LayerIndex;
			m_Stack.erase(it);

			if (!IsOverlay)
			{
				m_LayerIndex--;
			}

			Layer.OnDetach();

			if (Delete)
				delete &Layer;

			// Resume previous layer
			if (L* Old = GetLayer())
			{
				Old->Play();
			}

			return true;
		}

		void Clear()
		{
			int32_t Count = m_Stack.size();

			while (!m_Stack.empty())
			{
				Layer* Layer = *m_Stack.begin();

				// Might call pop, which deletes from stack
				delete Layer;

				if (Count == m_Stack.size())
				{
					m_Stack.erase(m_Stack.begin());
					Count = m_Stack.size();
				}
			}

			m_LayerIndex = 0;
		}
		
		// Removes all layers and replaces it with new specified layer
		template <class T, class... Args>
		void ClearAndReplace(Args... Arguments)
		{
			static_assert(
				std::is_base_of<L, T>::value,
				"Type is not a correct Layer"
			);

			Clear();
			Push<T>(Arguments...);
		}

		// Checks if layer exists within stack
		template <class T>
		bool Has() const
		{
			static_assert(
				std::is_base_of<L, T>::value,
				"Type is not a Layer"
			);

			for (L* Layer : m_Stack)
			{
				if (dynamic_cast<T*>(Layer))
					return true;
			}

			return false;
		}

		// Removes current layer and switches to the new specified layer
		template <class T, class... Args>
		void Switch(Args... Arguments)
		{
			static_assert(
				std::is_base_of<L, T>::value,
				"Type is not a Layer"
			);

			if (GetLayer())
				Pop(GetLayer());

			Push<T>(Arguments...);
		}

		// Gets the last inserted layer
		L* GetLayer()
		{
			if (m_LayerIndex <= 0) return nullptr;
			return m_Stack[m_LayerIndex - 1];
		}

		// Gets the last inserted layer + all overlays
		std::vector<L*> GetActive() const
		{
			return std::vector<L*>(
				m_Stack.begin() + m_LayerIndex,
				m_Stack.end()
			);
		}

		// Gets all inserted layers + overlays
		const std::vector<L*>& GetStack() const { return m_Stack; }

		// The amount of inserted layers (without overlays)
		uint32_t GetLayerCount() const { return m_LayerIndex; }

	protected:
		virtual bool Play()
		{
			// Play active layer + overlays after it
			for (Layer* Layer : GetActive())
			{
				Layer->Play();
			}

			return true;
		}

		virtual bool Pause()
		{
			// Pause active layer + overlays after it
			for (Layer* Layer : GetActive())
			{
				Layer->Pause();
			}

			return true;
		}

		virtual bool Resume()
		{
			// Resume active layer + overlays after it
			for (Layer* Layer : GetActive())
			{
				Layer->Resume();
			}

			return true;
		}

		virtual bool Stop()
		{
			// Stop active layer + overlays after it
			for (Layer* Layer : GetActive())
			{
				Layer->Stop();
			}

			return true;
		}

		std::vector<L*> m_Stack;
		uint32_t m_LayerIndex = 0;
	};
}
