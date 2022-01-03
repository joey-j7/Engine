#pragma once

#include "Engine/Objects/Stateable.h"

// A layer can be attached to one stack, as layer or overlay

namespace Engine
{
	class Engine_API Layer : protected Stateable
	{
		template<typename T>
		friend class Layerable;

	public:
		virtual ~Layer();

		virtual bool IsActive() const override { return Stateable::IsActive(); }
		virtual bool IsStopped() const override { return Stateable::IsStopped(); }
		virtual State GetState() const override { return Stateable::GetState(); }

		void Reset()
		{
			OnDetach();
			OnAttach();
		};

	protected:
		Layer();

		virtual void OnAttach() {};
		virtual void OnDetach() {};

	private:
		// Make new private, object will be initialized through Layerable
		void* operator new (std::size_t sz)
		{
			void* p = nullptr;

			/* malloc (0) is unpredictable; avoid it.  */
			if (sz == 0)
				sz = 1;

			while (__builtin_expect((p = malloc(sz)) == 0, false))
			{
				std::new_handler handler = std::get_new_handler();
				
				if (!handler)
					throw std::bad_alloc{};

				handler();
			}

			return p;
		}
	};
}